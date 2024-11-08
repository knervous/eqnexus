export module overlay;

import globals;
import d3d9_hooks;
import http;
import zipextractor;
import utility;
import hooks;
import filesystem;
import login;
import server;
import nk;

import <windows.h>;
import <d3d9.h>;
import <dinput.h>;
import <string>;
import <functional>;
import <vector>;
import <thread>;
import <filesystem>;
import <iostream>;
import "nuklear_d3d9.h";
import <rapidjson/document.h>;
import <fstream>;
import <filesystem>;

namespace fs = std::filesystem;

constexpr std::string_view ini_path = "eqnexus/config.ini";
constexpr std::string_view core_version = "0.0.1";

struct ServerInfo {
    std::string shortname = "";
    std::string longname = "";
    std::string url = "";
    std::string version = "";
    std::vector<std::string> hosts = {};
    int id = -1;
    bool up_to_date = false;
    bool downloading = false;
    std::string error = "";
    double pct_downloaded = 0.0;
    std::string status = "";
    void ValidateInstall() {
        const auto& root_path = "eqnexus/" + shortname;
        fs::path hash_path = root_path + "/hash.txt";
        fs::path version_path = root_path + "/version.txt";

        if (!fs::exists(root_path) || !fs::exists(hash_path) || !fs::exists(version_path)) {
            up_to_date = false;
            return;
        }
        unsigned long current_checksum = zipextractor::ComputeDirectoryTimestampChecksum(root_path);
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
    }

    void WriteHashAndVersion() {
        const auto& root_path = "eqnexus/" + shortname;
        const auto adler = zipextractor::ComputeDirectoryTimestampChecksum(root_path);
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
    EQOverlay() {
        D3D9Hooks::SetRenderCallback([this](IDirect3DDevice9* device) { OnRender(device); });
        D3D9Hooks::SetResetCallback([this](IDirect3DDevice9* device) { OnReset(device); });
        Login::SetLoginCallback([this](int server_id) { return DoLogin(server_id); });
    }

    ~EQOverlay() {
        D3D9Hooks::ClearRenderCallback();
        D3D9Hooks::ClearResetCallback();
    }

    void SetKeyboard(IDirectInputDevice8A* keyboard) { this->keyboard = keyboard; }
    void SetMouse(IDirectInputDevice8A* mouse) { this->mouse = mouse; }

private:
    void InitializeBounds() {
        RECT clientRect;
        FetchServerData();
        if (GetClientRect(GetActiveWindow(), &clientRect)) {
            window_width = 300;
            window_height = (clientRect.bottom - clientRect.top) / 2;
            window_x = (clientRect.right - clientRect.left) - (window_width + 10);
            window_y = 10;
        }
        else {
            window_width = 300;
            window_height = 500;
            window_x = 1200;
            window_y = 10;
        }
    }

    void FetchServerData() {
        status = "Fetching Server Metadata...";
        const auto url = util::ReadIniValue("ServerMetadata", "ManifestUrl", ini_path);
        const auto release_url = util::ReadIniValue("ServerMetadata", "ReleaseUrl", ini_path);
        servers.clear();

        if (auto response = http::DownloadJson(url); !response.empty()) {
            rapidjson::Document document;
            document.Parse(response.c_str());
            if (!document.HasParseError()) {
                auto manifest_version = std::string(document["version"].GetString());
                if (core_version != manifest_version && !version_warning_shown) {
                    version_warning_shown = true;
                    std::thread t([this, manifest_version, release_url]() {
                        while (!Login::DidRetrieveServers()) {
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        }
                        // TODO investigate anchor links crashing ResetDevice--otherwise like the idea of embedded links
                        // ShowPopup("Core version out of date. Latest version is: " + manifest_version + "<br></br><br></br>To download the latest core version, please visit " + "<a href=\"" + release_url + "\">" + release_url + "</a>");
                        ShowPopup("Core version out of date. Latest version is: " + manifest_version + "<br></br><br></br>To download the latest core version, please visit " + release_url);
                    });
                    t.detach();
                }

                status = "Server Manifest Version: " + manifest_version;
                for (const auto& server : document["servers"].GetArray()) {
                    ServerInfo info{
                        server["shortName"].GetString(),
                        server["longName"].GetString(),
                        server["customFilesUrl"].GetString(),
                        server["version"].GetString()
                    };

                    for (const auto& host : server["hosts"].GetArray()) {
                        if (host.IsString()) info.hosts.push_back(host.GetString());
                    }
                    info.ValidateInstall();
                    servers.push_back(info);
                }
            }
            else {
                status = "Error from URL: " + std::string(url);
            }
        }
    }

    void DownloadServerFilesTask(ServerInfo& server) {
        if (server.url.empty()) {
            server.error = "No URL defined";
            task_running = false;
            return;
        }

        const auto path = "eqnexus/" + server.shortname;
        if (!fs::exists(path) && !fs::create_directories(path)) {
            ShowPopup("Filesystem Error. Unable to create directory: " + path);
            task_running = false;
            return;
        }

        auto zip_path = path + "/" + server.shortname + ".zip";
        server.downloading = true;

        if (http::DownloadBinary(server.url, zip_path, [&server](double pct) { server.pct_downloaded = pct; })) {
            server.downloading = false;

            if (zipextractor::ExtractAllFilesFromZip(zip_path, [this, &server](const std::string& filename) {
                server.status = "Extracting " + filename;
            }) && fs::remove(zip_path)) {
                server.WriteHashAndVersion();
            }
            else {
                ShowPopup("Extract Error. Error extracting zip from: " + zip_path);
            }
        }
        else {
            server.error = "HTTP Failure";
        }
        server.status = "";
        server.downloading = false;
        server.ValidateInstall();
        task_running = false;
    }

    inline void DownloadServerFiles(ServerInfo& server) {
        task_running = true;
        std::thread(&EQOverlay::DownloadServerFilesTask, this, std::ref(server)).detach();
    }


    void OnRender(IDirect3DDevice9* device) {
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
        struct nk_rect window_rect_popup = nk_rect(window_x - window_width - 20, window_y, window_width, 150);

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
                nk_util::DrawSeparator(ctx);
                nk_layout_row_dynamic(ctx, 20, 2);
                if (nk_widget_is_hovered(ctx)) {
                    nk_tooltip(ctx, server.longname.c_str());
                }
                nk_label(ctx, nk_util::TruncateTextWithEllipsis(ctx, server.longname.c_str(), 200.0f).c_str(), NK_TEXT_LEFT);
                nk_label(ctx, server.version.c_str(), NK_TEXT_RIGHT);

                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, ("Path: " + server.shortname).c_str(), NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 20, 1);
                const auto& url = (server.url.empty() ? std::string("None") : server.url);
                if (nk_widget_is_hovered(ctx)) {
                    nk_tooltip(ctx, url.c_str());
                }
                nk_label(ctx, ("URL: " + nk_util::TruncateTextWithEllipsis(ctx, url.c_str(), 200.0f)).c_str(), NK_TEXT_LEFT);
               
                bool server_task_running = !server.status.empty() || server.downloading;
                nk_layout_row_dynamic(ctx, 25, server_task_running ? 1 : 2);
                std::string status = !server.status.empty() ? server.status : 
                    server.downloading ?
                    util::ToStringWithPrecision(server.pct_downloaded) + "% Downloaded" : !server.error.empty() ?
                    server.error : server.up_to_date ?
                    "Up to date" : "Needs update";
                nk_label(ctx, ("Status: " + status).c_str(), NK_TEXT_LEFT);
                if (!task_running && !server.up_to_date) {
                    if (nk_button_label(ctx, "Download")) {
                        DownloadServerFiles(server);
                    }
                }
                if (server.up_to_date && nk_button_label(ctx, "Open Files")) {
                    util::OpenExplorerInCustomDirectory(server.shortname);
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

#ifdef _DEBUG
        struct nk_rect debug_wnd_rect = nk_rect(0.0f, 0.0f, 200.0f, 300.0f);
        if (nk_begin(ctx, "Debug", debug_wnd_rect,
            NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
            nk_layout_row_dynamic(ctx, 30, 1);

            nk_checkbox_label(ctx, "Skip validation", &skip_validation);
            float ww = nk_window_get_content_region(ctx).w;
            float button_width = 135.0f;
            float x_pos = (ww - button_width) / 2;

            nk_layout_space_begin(ctx, NK_STATIC, 40, 1);
            nk_layout_space_push(ctx, nk_rect(x_pos, 0, button_width, 30));


            nk_layout_space_end(ctx);
            nk_layout_space_begin(ctx, NK_STATIC, 40, 1);
            nk_layout_space_push(ctx, nk_rect(x_pos, 0, button_width, 30));

            if (nk_button_label(ctx, "Test")) {
                Login::OpenModal("Testing here: <a href=\"https://google.com\">Test anchor</a>");
            }

            nk_layout_space_end(ctx);

        }
        nk_end(ctx);

#endif
        nk_d3d9_render(NK_ANTI_ALIASING_ON);
    }

    void InitNuklearCtx(IDirect3DDevice9* device) {
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

    void OnReset(IDirect3DDevice9* device) {
        auto state = GetGameState();
        if (nuklear_initialized) {
            nk_d3d9_shutdown();
            nuklear_initialized = false;
            if (state > -1) {
                return;
            }
        }
        InitNuklearCtx(device);
    }

    void PollDirectInputForNuklear() {
        if (mouse && SUCCEEDED(mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state))) {
            if (POINT cursorPos; GetCursorPos(&cursorPos)) {
                ScreenToClient(GetActiveWindow(), &cursorPos);
                mouse_x = cursorPos.x;
                mouse_y = cursorPos.y;
            }
            nk_input_motion(ctx, mouse_x, mouse_y);
            nk_input_button(ctx, NK_BUTTON_LEFT, mouse_x, mouse_y, (mouse_state.rgbButtons[0] & 0x80) != 0);
            nk_input_button(ctx, NK_BUTTON_RIGHT, mouse_x, mouse_y, (mouse_state.rgbButtons[1] & 0x80) != 0);
        }
    }

    int GetGameState() { return eqlib::pEverQuest ? eqlib::pEverQuest->GameState : -1; }
    bool DoLogin(int server_id) {
        for (auto& server : servers) {
            for (const auto& s : eqlib::g_pLoginClient->ServerList) {
                if (s->ID != server_id) {
                    continue;
                }
                for (const auto& host : server.hosts) {
                    auto server_host = std::string(s->HostName.c_str());
                    if (s->HostName.c_str() == host) {
                        server.ValidateInstall();
#ifdef _DEBUG
                        if (!skip_validation && !server.up_to_date) {
#else
                        if (!server.up_to_date) {
#endif
                            Login::InterceptUnknown("<b>Failed to join custom server<br></br>Your patch files are out of date for " + server.longname + ".");
                            Server::SetContext("");
                            return true;
                        }
                        Server::SetContext(server.shortname);
                        return false;
                        }
                    }
                }
            }
        Server::SetContext("");
        return false;
    }

    void ShowPopup(const std::string& msg) { Login::OpenModal(msg); }

    IDirectInputDevice8A* keyboard = nullptr;
    IDirectInputDevice8A* mouse = nullptr;
    DIMOUSESTATE mouse_state{};
    char keyboard_state[256]{};
    int mouse_x = 0;
    int mouse_y = 0;
    float window_x = 0xFFFF;
    float window_y = 0xFFFF;
    float window_width = 300;
    float window_height = 200;
    bool nuklear_initialized = false;
    nk_context* ctx = nullptr;
    nk_font_atlas* atlas = nullptr;
    nk_font* header_font;
    nk_font* body_font;
    nk_color bg = { 2, 5, 16 };
    nk_color border = nk_rgb(198, 170, 132);
    std::string status = "Initializing";
    std::vector<ServerInfo> servers{};
    bool task_running = false;
    bool version_warning_shown = false;
#ifdef _DEBUG
    nk_bool skip_validation = false;
#endif
};
