export module overlay;

import globals;
import d3d9_hooks;
import http;
import zipextractor;

import <windows.h>;
import <d3d9.h>;
import <dinput.h>;
import <string>;
import <functional>;
import <vector>;
import <thread>;
import <filesystem>;
import <iostream>;
import <span>;
import "nuklear_d3d9.h";
import <rapidjson/document.h>;
import <fstream>;
import <filesystem>;

namespace fs = std::filesystem;

constexpr const char* iniPath = "eqnexus/config.ini";

std::string ReadIniValue(std::string_view section, std::string_view key, const std::filesystem::path& iniFilePath);
void DrawSeparator(struct nk_context* ctx);

struct ServerInfo {
    std::string shortname = "";
    std::string longname = "";
    std::string url = "";
    std::string version = "";
    std::vector<std::string> hosts = {};
    bool up_to_date = false;
    bool downloading = false;
    std::string error = "";
    double pct_downloaded = 0.0;

    void ValidateInstall() {
        const auto& root_path = "eqnexus/" + shortname;
        fs::path hash_path = root_path + "/hash.txt";
        fs::path version_path = root_path + "/version.txt";

        if (!fs::exists(root_path) || !fs::exists(hash_path) || !fs::exists(version_path)) {
            up_to_date = false;
            return;
        }
        unsigned long current_checksum = zipextractor::ComputeDirectoryChecksum(root_path);
        unsigned long stored_checksum = 0;
        std::ifstream hash_file(hash_path);
        hash_file >> stored_checksum;
        if (current_checksum != stored_checksum) {
            up_to_date = false;
            return;
        }
        std::string stored_version;
        std::ifstream version_file(version_path);
        version_file >> stored_version;
        if (version != stored_version) {
            up_to_date = false;
            return;
        }
        // We did it!
        up_to_date = true;
    };
    void WriteHashAndVersion() {
        const auto& root_path = "eqnexus/" + shortname;
        const auto adler = zipextractor::ComputeDirectoryChecksum(root_path);
        fs::path hash_path = root_path + "/hash.txt";
        std::ofstream hash_out(hash_path, std::ios::trunc);
        hash_out << adler;
        fs::path version_path = root_path + "/version.txt";
        std::ofstream version_out(version_path, std::ios::trunc);
        version_out << version;
    }
};

export class EQOverlay {
public:
    EQOverlay();
    void SetKeyboard(IDirectInputDevice8A* keyboard) { this->keyboard = keyboard; }
    void SetMouse(IDirectInputDevice8A* mouse) { this->mouse = mouse; }
    void Reset();

private:
    void InitializeBounds();
    void FetchServerData();
    void OnRender(IDirect3DDevice9* device);
    void OnReset(IDirect3DDevice9* device);
    void PollDirectInputForNuklear();
    void DownloadServerFiles(ServerInfo& server);
    void InitNuklearCtx(IDirect3DDevice9* device);
    int GetGameState();
    void ShowPopup(const std::string& title, const std::string& msg) {
        popup_title = title;
        popup_msg = msg;
        show_popup = true;
    }
    void ClearPopup() {
        popup_title = "";
        popup_msg = "";
        show_popup = false;
    }

    IDirectInputDevice8A* keyboard = nullptr;
    IDirectInputDevice8A* mouse = nullptr;
    DIMOUSESTATE mouse_state = {};
    char keyboard_state[256] = {};
    int mouse_x = 0;
    int mouse_y = 0;
    int window_x = 0xFFFF;
    int window_y = 0xFFFF;
    int window_width = 300;
    int window_height = 200;

    bool nuklear_initialized = false;
    struct nk_context* ctx = nullptr;
    struct nk_font_atlas* atlas = nullptr;
    struct nk_font* header_font;
    struct nk_font* body_font;
    struct nk_color bg = { 2, 5, 16 };
    struct nk_color border = nk_rgb(198, 170, 132);

    // Status
    std::string status = "Initializing";
    std::vector<ServerInfo> servers = {};

    bool task_running = false;

    // Popup dialog
    bool show_popup = false;
    std::string popup_title = "";
    std::string popup_msg = "";
};

// Helper function implementations
void DrawSeparator(struct nk_context* ctx) {
    struct nk_color yellow = nk_rgb(198, 170, 132);
    float separator_width = 0.8f;

    nk_layout_row_dynamic(ctx, 5, 1);
    nk_label(ctx, "", NK_TEXT_LEFT);
    nk_layout_row_begin(ctx, NK_STATIC, 1, 1);
    nk_layout_row_push(ctx, ctx->current->layout->bounds.w * separator_width);
    struct nk_rect rect = nk_layout_widget_bounds(ctx);
    rect.h = 1;
    nk_fill_rect(&ctx->current->buffer, rect, 0, yellow);
    nk_layout_row_end(ctx);

    nk_layout_row_dynamic(ctx, 5, 1);
    nk_label(ctx, "", NK_TEXT_LEFT);
}

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

EQOverlay::EQOverlay() {
    D3D9Hooks::SetRenderCallback([&](IDirect3DDevice9* device) {
        this->OnRender(device);
    });
    D3D9Hooks::SetResetCallback([&](IDirect3DDevice9* device) {
        this->OnReset(device);
    });
    FetchServerData();
}

void EQOverlay::Reset() {
    D3D9Hooks::ClearRenderCallback();
    D3D9Hooks::ClearResetCallback();
    nk_free(ctx);
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
    else {
        window_width = 300;
        window_height = 500;
        window_x = 1200;
        window_y = 10;
    }
}

void EQOverlay::FetchServerData() {
    status = "Fetching Server Metadata...";
    const auto& url = ReadIniValue("ServerMetadata", "ManifestUrl", iniPath);
    servers.clear();
    auto response = http::DownloadJson(url);
    rapidjson::Document document;
    document.Parse(response.c_str());
    if (document.HasParseError()) {
        status = "Error from URL: " + url;
        return;
    }
    if (document.HasMember("version") && document["version"].IsString()) {
        std::string version = document["version"].GetString();
        status = "Server Manifest Version: " + version;
        std::cout << "Version: " << version << std::endl;
    }
    if (document.HasMember("servers") && document["servers"].IsArray()) {
        const rapidjson::Value& servers = document["servers"];

        for (const auto& server : servers.GetArray()) {
            ServerInfo info{
                server["shortName"].GetString(),
                server["longName"].GetString(),
                server["customFilesUrl"].GetString(),
                server["version"].GetString(),
            };

            if (server.HasMember("hosts") && server["hosts"].IsArray()) {
                const rapidjson::Value& hosts = server["hosts"];
                for (const auto& host : hosts.GetArray()) {
                    if (host.IsString()) {
                        info.hosts.push_back(host.GetString());
                    }
                }
            }
            info.ValidateInstall();
            this->servers.push_back(info);
        }
    }
}

void EQOverlay::DownloadServerFiles(ServerInfo& server) {
    task_running = true;
    std::thread t([this, &server]() {
        if (!server.url.empty()) {
            const auto& path = "eqnexus/" + server.shortname;
            if (!fs::exists(path)) {
                if (fs::create_directories(path)) {
                    std::cout << "Directory created: " << path << std::endl;
                } else {
                    ShowPopup("Filesystem Error", "Was unable to create directory: " + path);
                    task_running = false;
                    return;
                }
            }
            auto zip_path = path + "/" + server.shortname + ".zip";
            {
                server.downloading = true;
            }
            if (http::DownloadBinary(server.url, zip_path, [this, &server](double pct) {
         
            })) {
                if (zipextractor::ExtractAllFilesFromZip(zip_path) && fs::remove(zip_path)) {
                    server.WriteHashAndVersion();
                }
                else {
                    ShowPopup("Extract Error", "Error extracting zip from: " + zip_path);
                }
            }
            else {
                server.error = "HTTP Failure";
            }
            server.downloading = false;
            server.ValidateInstall();
        }
        else {
            server.error = "No URL defined";
        }
  
        task_running = false;
    });
    t.detach();
}

void EQOverlay::OnRender(IDirect3DDevice9* device) {
    if (GetGameState() != -1) {
        return;
    }
    if (!nuklear_initialized) {
        InitNuklearCtx(device);
    }

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
        nk_style_pop_font(ctx);
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, status.c_str(), NK_TEXT_CENTERED);
        float ww = nk_window_get_content_region(ctx).w;
        float button_width = 135.0f;
        float x_pos = (ww - button_width) / 2;

        nk_layout_space_begin(ctx, NK_STATIC, 40, 1);
        nk_layout_space_push(ctx, nk_rect(x_pos, 0, button_width, 30));

        if (nk_button_label(ctx, "Refresh Server List")) {
            std::thread t(std::bind(&EQOverlay::FetchServerData, this));
            t.detach();
        }

        nk_layout_space_end(ctx);

        for (auto& server : servers) {
            DrawSeparator(ctx);
            nk_layout_row_dynamic(ctx, 20, 2);
            nk_label(ctx, server.longname.c_str(), NK_TEXT_LEFT);
            nk_label(ctx, server.shortname.c_str(), NK_TEXT_RIGHT);

            nk_layout_row_dynamic(ctx, 25, 2);
            std::string status = server.downloading ? 
                "Downloading..." : !server.error.empty() ?
                    server.error : server.up_to_date ?
                        "Up to date" : "Needs update";
            nk_label(ctx, ("Status: " + status).c_str(), NK_TEXT_LEFT);
            if (!task_running && !server.up_to_date) {
                if (nk_button_label(ctx, "Download")) {
                    DownloadServerFiles(server);
                }
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

        struct nk_style* s = &ctx->style;
        auto text_color = nk_rgb(240, 240, 240);
        s->text.color = text_color;
        s->window.border_color = border;
        s->window.border = 1.5f;
        s->window.header.label_normal = s->window.header.label_hover = s->window.header.label_active = nk_rgb(255, 218, 96);
        s->window.header.align = NK_HEADER_RIGHT;
        s->window.header.normal = s->window.header.active = nk_style_item_color(nk_rgb(41, 56, 74));
        s->button.text_active = border;
        s->button.text_hover = nk_rgb(255, 218, 96);
        s->button.text_normal = text_color;
        s->button.normal = nk_style_item_color(nk_rgb(41, 56, 74));
        s->button.hover = nk_style_item_color(nk_rgb(30, 30, 50));
        s->button.active = nk_style_item_color(nk_rgb(22, 22, 55));
        s->button.border_color = border;
        s->window.padding = nk_vec2(10, 10);
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
