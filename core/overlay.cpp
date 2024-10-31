#include "pch.h"
#include "overlay.h"
#include <string>
#include <string_view>
#include <filesystem>
#include <iostream>
#include <span>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>

#include "eq/globals.h"

using namespace web;       // Common namespace for JSON and URI
using namespace web::http; // HTTP request and response
using namespace web::http::client; // HTTP client

constexpr const char* iniPath = "eqnexus/config.ini";

std::string ReadIniValue(std::string_view section, std::string_view key, const std::filesystem::path& iniFilePath) {
	constexpr size_t bufferSize = 256;
	char buffer[bufferSize]{};
	std::span<char> bufferSpan(buffer);

	GetPrivateProfileStringA(
		section.data(),
		key.data(),
		"",
		bufferSpan.data(),
		static_cast<DWORD>(bufferSpan.size()),
		iniFilePath.string().c_str()
	);

	return std::string(buffer);
}

int EQOverlay::GetGameState() {
	return eqlib::pEverQuest ? eqlib::pEverQuest->GameState : -1;
}
void EQOverlay::InitializeBounds() {
	RECT clientRect;
	if (GetClientRect(GetActiveWindow(), &clientRect)) {
		int clientWidth = clientRect.right - clientRect.left;
		int clientHeight = clientRect.bottom - clientRect.top;
		const int padding = 10;
		window_width = 300;
		window_height = clientHeight / 2;
		window_x = clientWidth - (window_width + padding);
		window_y = padding;
	}
}

void EQOverlay::FetchServerData() {
	status = "Fetching Server Metadata...";
	const auto& url = ReadIniValue("ServerMetadata", "ManifestUrl", iniPath);
	status = url;
	http_client client(utility::conversions::to_string_t(url));
	servers.clear();
	client.request(methods::GET)
		.then([](http_response response) {
		if (response.status_code() == status_codes::OK) {
			return response.extract_json(true);
		}
		else {
			throw std::runtime_error("Failed to fetch JSON");
		}
	})
		.then([this](json::value jsonResponse) {
		// Parse and output the JSON content
		std::wcout << L"JSON response:\n" << jsonResponse.serialize() << std::endl;
		
		// Access individual properties in the JSON
		if (jsonResponse.is_object()) {
			auto jsonObject = jsonResponse.as_object();
			status = "Server Manifest Version: " + std::to_string(jsonObject[U("version")].as_double());

			if (jsonObject[U("servers")].is_array()) {
				for (const auto& server : jsonObject[U("servers")].as_array()) {
					const auto& serverObject = server.as_object();
					ServerInfo info{
						utility::conversions::to_utf8string(serverObject.at(U("shortName")).as_string()),
						utility::conversions::to_utf8string(serverObject.at(U("longName")).as_string()),
						utility::conversions::to_utf8string(serverObject.at(U("customFilesUrl")).as_string()),
					};
					servers.push_back(info);
				}
			}
		}
	}).then([this, url](pplx::task<void> previousTask) {
		try {
			// Wait for the previous task and catch any exceptions
			previousTask.wait();
		}
		catch (const std::exception& e) {
			// Handle any errors here
			std::cerr << "Error: " << e.what() << std::endl;
			status = "An error occurred fetching and parsing URL: " + url;
		}
	})
		.wait(); // Wait for all tasks to complete

	
}

void EQOverlay::OnRender(IDirect3DDevice9* device) {
	if (GetGameState() != -1) {
		return;
	}
	if (!nuklear_initialized) {
		InitNuklearCtx(device);
	}

	// Bounds check
	if (window_x == 0xFFFF) {
		InitializeBounds();
	}
	nk_input_begin(ctx);
	PollDirectInputForNuklear();
	nk_input_end(ctx);
	struct nk_style* s = &ctx->style;
	nk_style_push_style_item(ctx, &s->window.fixed_background, nk_style_item_color(nk_rgb(bg.r, bg.g, bg.b)));

	struct nk_rect window_rect = nk_rect(window_x, window_y, window_width, window_height);

	nk_style_push_font(ctx, &header_font->handle);
	if (nk_begin(ctx, "EQ Nexus Server Patcher", window_rect,
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
		NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
		// Set header font for the titlebar
		nk_style_pop_font(ctx);

		enum { EASY, HARD };
		static int op = EASY;
		static int property = 20;

		nk_layout_row_dynamic(ctx, 40, 1);
		nk_label(ctx, status.c_str(), NK_TEXT_CENTERED);

		for (const auto& server : servers) {
			nk_layout_row_static(ctx, 20, 120, 2);
			nk_label(ctx, server.longname.c_str(), NK_TEXT_LEFT);
			nk_label(ctx, server.shortname.c_str(), NK_TEXT_RIGHT);
			nk_layout_row_static(ctx, 20, 120, 1);
			nk_label(ctx, "Status: Needs patch", NK_TEXT_LEFT);
			nk_layout_row_static(ctx, 20, 120, 1);

			if (nk_button_label(ctx, "Download")) {
			
			
			}


		}
	}
	else {
		nk_style_pop_font(ctx);
	}
	nk_style_pop_style_item(ctx);

	struct nk_vec2 new_position = nk_window_get_position(ctx);
	struct nk_vec2 new_size = nk_window_get_size(ctx);
	window_x = new_position.x;
	window_y = new_position.y;
	window_width = new_size.x;
	window_height = new_size.y;
	nk_end(ctx);
	nk_d3d9_render(NK_ANTI_ALIASING_ON);
}

void EQOverlay::InitNuklearCtx(IDirect3DDevice9* device) {
	D3DVIEWPORT9 viewport;
	if (SUCCEEDED(device->GetViewport(&viewport))) {
		ctx = nk_d3d9_init(device, viewport.Width, viewport.Height);
		nk_d3d9_font_stash_begin(&atlas);
		body_font = nk_font_atlas_add_from_file(atlas, "eqnexus/Roboto-Regular.ttf", 17, 0);
		header_font = nk_font_atlas_add_from_file(atlas, "eqnexus/Raleway-Bold.ttf", 16, 0);
		nk_d3d9_font_stash_end();
		nk_style_load_all_cursors(ctx, atlas->cursors);
		nk_style_set_font(ctx, &body_font->handle);

		// Global styles
		struct nk_style* s = &ctx->style;
		auto text_color = nk_rgb(240, 240, 240);
		s->text.color = text_color;
		s->window.border_color = border;
		s->window.border = 1.5f;
		s->window.header.label_normal = nk_rgb(255, 218, 96); // Normal title color
		s->window.header.label_hover = nk_rgb(255, 218, 96);  // Hover title color
		s->window.header.label_active = nk_rgb(255, 218, 96); // Active (focused) title color
		s->window.header.align = NK_HEADER_RIGHT;
		s->window.header.normal = s->window.header.active = nk_style_item_color(nk_rgb(2, 2, 15));
		s->button.text_active = text_color;
		s->button.border_color = text_color;
		s->window.padding = nk_vec2(10, 10); // Adds 10px padding around all sides of each widget
		s->window.spacing = nk_vec2(5, 5);

		nuklear_initialized = true;
	}
}

void EQOverlay::OnReset(IDirect3DDevice9* device) {
	if (nuklear_initialized) {
		nk_d3d9_shutdown();
		nuklear_initialized = false;
		if (GetGameState() > -1) {
			return;
		}
	}
	InitNuklearCtx(device);
}

void EQOverlay::PollDirectInputForNuklear() {
	if (keyboard) {
		if (SUCCEEDED(keyboard->GetDeviceState(sizeof(keyboard_state), (LPVOID)&keyboard_state))) {
			// Process keyboard state here
		}
	}

	if (mouse) {
		if (SUCCEEDED(mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state))) {
			POINT cursorPos;
			if (GetCursorPos(&cursorPos)) {
				ScreenToClient(GetActiveWindow(), &cursorPos);
				mouse_x = cursorPos.x;
				mouse_y = cursorPos.y;
			}
			nk_input_motion(ctx, mouse_x, mouse_y);
			nk_input_button(ctx, NK_BUTTON_LEFT, mouse_x, mouse_y, (mouse_state.rgbButtons[0] & 0x80) != 0);
			nk_input_button(ctx, NK_BUTTON_RIGHT, mouse_x, mouse_y, (mouse_state.rgbButtons[1] & 0x80) != 0);
		}
	}
}