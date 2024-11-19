export module overlay;

import globals;
import d3d9_hooks;
import http;
import utility;
import hooks;
import filesystem;
import login;
import server;
import nk;
import server_info;
import updater;
import config;

import <windows.h>;
import <d3d9.h>;
import <dinput.h>;
import <string>;
import <functional>;
import <vector>;
import <memory>;
import <thread>;
import <filesystem>;
import <iostream>;
import "nuklear_d3d9.h";
import <rapidjson/document.h>;
import <fstream>;

namespace fs = std::filesystem;

constexpr std::string_view ini_path = "eqnexus/config.ini";

export class EQOverlay
{
   public:
    EQOverlay()
    {
        D3D9Hooks::SetRenderCallback([this](IDirect3DDevice9* device) { OnRender(device); });
        D3D9Hooks::SetResetCallback([this](IDirect3DDevice9* device) { OnReset(device); });
        Login::SetLoginCallback([this](int server_id) { return DoLogin(server_id); });
    }

    ~EQOverlay()
    {
        D3D9Hooks::ClearRenderCallback();
        D3D9Hooks::ClearResetCallback();
    }

    void SetKeyboard(IDirectInputDevice8A* keyboard)
    {
        this->keyboard = keyboard;
    }
    void SetMouse(IDirectInputDevice8A* mouse)
    {
        this->mouse = mouse;
    }

   private:
    bool server_op                 = false;
    IDirectInputDevice8A* keyboard = nullptr;
    IDirectInputDevice8A* mouse    = nullptr;
    IDirect3DTexture9* bg_texture  = nullptr;
    DIMOUSESTATE mouse_state{};
    char keyboard_state[256]{};
    int mouse_x                     = 0;
    int mouse_y                     = 0;
    float window_x                  = 0xFFFF;
    float window_y                  = 0xFFFF;
    float window_width              = 300;
    float window_height             = 200;
    bool nuklear_initialized        = false;
    std::string file_processed      = "";
    std::string release_url         = util::ReadIniValue("ServerMetadata", "ReleaseUrl", ini_path);
    std::string core_version        = util::ReadIniValue("Version", "CoreVersion", ini_path);
    std::string latest_core_version = "";
    nk_context* ctx                 = nullptr;
    nk_font_atlas* atlas            = nullptr;
    nk_font* header_font;
    nk_font* body_font;
    nk_color bg        = {2, 5, 16};
    nk_color border    = nk_rgb(198, 170, 132);
    std::string status = "Initializing";
    std::vector<std::unique_ptr<ServerInfo>> servers{};
    CoreVersion version;
    bool task_running          = false;
    bool version_warning_shown = false;
    bool has_latest            = true;
#ifdef DEV
    nk_bool skip_validation = false;
#endif

    void InitializeBounds()
    {
        RECT clientRect;
        if (GetClientRect(GetActiveWindow(), &clientRect))
        {
            window_width  = 330;
            window_height = ((clientRect.bottom - clientRect.top) / 2);
            window_x      = (clientRect.right - clientRect.left) - (window_width + 10);
            window_y      = 10;
        }
        else
        {
            window_width  = 300;
            window_height = 500;
            window_x      = 1200;
            window_y      = 10;
        }
    }

    void FetchServerData()
    {
        status                = "Fetching Server Metadata...";
        const auto url        = util::ReadIniValue("ServerMetadata", "ManifestUrl", ini_path);
        const auto latest_url = util::ReadIniValue("ServerMetadata", "LatestUrl", ini_path);
        server_op             = util::ReadIniValue("ServerMetadata", "ServerOp", ini_path) == "true";
        servers.clear();
        std::thread t([this, url, latest_url]() {
            if (auto response = http::DownloadJson(url), latest_response = http::DownloadJson(latest_url);
                !response.empty(), !latest_response.empty())
            {
                rapidjson::Document document;
                document.Parse(response.c_str());
                rapidjson::Document latest_document;
                latest_document.Parse(latest_response.c_str());
                if (document.HasParseError() || latest_document.HasParseError() || latest_document.HasMember("error"))
                {
                    std::thread t([this]() {
                        while (!Login::DidRetrieveServers())
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        }
                        ShowPopup(
                            "There was an error downloading the server manifest. <br></br><br></br>To download the latest core version, "
                            "please visit <a href=\"" +
                            release_url + "\">" + release_url + "</a>");
                    });
                    t.detach();
                    status = "Error fetching metadata";
                    return;
                }

                latest_core_version = std::string(latest_document["version"].GetString());
                auto filename       = std::string(latest_document["filename"].GetString());

                version = CoreVersion{latest_core_version, filename};

                if (core_version != latest_core_version && !version_warning_shown)
                {
                    version_warning_shown = true;
                    std::thread t([this]() {
                        while (!Login::DidRetrieveServers())
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        }
                        ShowPopup("EQ Nexus Core version out of date. Latest version is: " + latest_core_version +
                                  "<br></br><br></br>To download the latest core version, please visit " + "<a href=\"" + release_url +
                                  "\">" + release_url + "</a> or click 'Update Core'");
                    });
                    t.detach();
                    has_latest = false;
                }
                else
                {
                    has_latest = true;
                }

                status = "";
                for (const auto& server : document["servers"].GetArray())
                {
                    if (server.HasMember("manifest") && server["manifest"].IsString())
                    {
                        if (auto serverResponse = http::DownloadJson(server["manifest"].GetString()); !serverResponse.empty())
                        {
                            rapidjson::Document serverDocument;
                            serverDocument.Parse(serverResponse.c_str());
                            if (serverDocument.HasParseError())
                            {
                                continue;
                            }

                            std::vector<std::string> hosts                     = {};
                            std::vector<std::string> required                  = {};
                            std::unordered_map<std::string, std::string> files = {};
                            for (const auto& host : serverDocument["hosts"].GetArray())
                            {
                                if (host.IsString())
                                    hosts.push_back(host.GetString());
                            }
                            if (serverDocument.HasMember("required") && serverDocument["required"].IsArray())
                            {
                                for (const auto& required_file : serverDocument["required"].GetArray())
                                {
                                    if (required_file.IsString())
                                        required.push_back(required_file.GetString());
                                }
                            }

                            if (serverDocument.HasMember("files") && serverDocument["files"].IsObject())
                            {
                                for (const auto& [name, hash] : serverDocument["files"].GetObj())
                                {
                                    if (name.IsString() && hash.IsString())
                                    {
                                        files[name.GetString()] = hash.GetString();
                                    }
                                }
                            }
                            auto getStringOrDefault = [&](const char* key, const std::string& defaultValue) -> std::string {
                                return serverDocument.HasMember(key) && serverDocument[key].IsString() ? serverDocument[key].GetString()
                                                                                                       : defaultValue;
                            };

                            servers.emplace_back(std::make_unique<ServerInfo>(getStringOrDefault("shortName", ""),
                                                                              getStringOrDefault("longName", ""),
                                                                              getStringOrDefault("customFilesUrl", ""),
                                                                              getStringOrDefault("filesUrlPrefix", ""),
                                                                              getStringOrDefault("version", ""),
                                                                              getStringOrDefault("website", "None"),
                                                                              getStringOrDefault("description", "None"),
                                                                              std::move(hosts),
                                                                              std::move(required),
                                                                              std::move(files)));
                            servers.back()->ValidateInstallAsync();
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
            }
        });
        t.detach();
    }

    void OnRender(IDirect3DDevice9* device)
    {
        if (GetGameState() != -1)
        {
            return;
        }
        if (!nuklear_initialized)
        {
            InitNuklearCtx(device);
        }

        if (window_x == 0xFFFF)
        {
            InitializeBounds();
        }
        nk_input_begin(ctx);
        PollDirectInputForNuklear();
        nk_input_end(ctx);
        struct nk_style* s = &ctx->style;
        // nk_style_push_style_item(ctx, &s->window.fixed_background, nk_style_item_color(nk_rgb(bg.r, bg.g, bg.b)));

        struct nk_rect window_rect       = nk_rect(window_x, window_y, window_width, window_height);
        struct nk_rect window_rect_popup = nk_rect(window_x - window_width - 20, window_y, window_width, 150);

        std::string msg("EQ Nexus");
        msg += util::Interpolate(" (version {})", core_version);
        nk_style_push_font(ctx, &header_font->handle);
        if (nk_begin(ctx,
                     msg.c_str(),
                     window_rect,
                     NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            nk_style_pop_font(ctx);

            if (!has_latest)
            {
                nk_layout_row_dynamic(ctx, 25, 1);
                std::string update_text("Update To Version " + latest_core_version);
                if (nk_button_label(ctx, update_text.c_str()))
                {
                    CoreUpdater::UpdateCore(version, release_url);
                }
            }

            nk_layout_row_dynamic(ctx, 25, 2);
            bool processing = !file_processed.empty();
            bool validating = false;
            for (const auto& server : servers)
            {
                validating = validating || server->IsValidating();
            }
            nk_style_push_vec2(ctx, &s->button.padding, nk_vec2(15.0f, s->button.padding.y));

            if (processing || task_running || validating)
            {
                nk_util::RenderDisabledButton(ctx, "Refresh");
            }
            else
            {
                if (nk_button_label(ctx, "Refresh"))
                {
                    FetchServerData();
                }
            }
            if (nk_button_label(ctx, "Website"))
            {
                HINSTANCE result = ShellExecuteA(nullptr, "open", release_url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }

            nk_style_pop_vec2(ctx);
            if (!status.empty())
            {
                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, status.c_str(), NK_TEXT_CENTERED);
            }

            if (server_op)
            {
                if (!processing && nk_button_label(ctx, "Generate Manifest"))
                {
                    ServerInfo info;
                    info.CreateManifest([this](const std::string& name) { file_processed = name.empty() ? "" : "Hashing: " + name; });
                }
                if (!file_processed.empty())
                {
                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, file_processed.c_str(), NK_TEXT_CENTERED);
                }
            }

            nk_layout_space_end(ctx);

            if (!processing)
            {
                for (auto& server : servers)
                {
                    if (server_op)
                    {
                        nk_layout_row_dynamic(ctx, 20, 1);
                        if (nk_button_label(ctx, ("Generate Manifest: " + server->GetShortName()).c_str()))
                        {
                            server->CreateManifest(
                                [this](const std::string& name) { file_processed = name.empty() ? "" : "Hashing: " + name; });
                        }
                    }
                    server->DrawListItem(ctx, task_running);
                }
            }
        }
        else
        {
            nk_style_pop_font(ctx);
        }
        // nk_style_pop_style_item(ctx);

        struct nk_vec2 new_position = nk_window_get_position(ctx);
        struct nk_vec2 new_size     = nk_window_get_size(ctx);
        window_x                    = new_position.x;
        window_y                    = new_position.y;
        window_width                = new_size.x;
        window_height               = new_size.y;
        nk_end(ctx);

#ifdef DEV
        /*struct nk_rect debug_wnd_rect = nk_rect(0.0f, 0.0f, 200.0f, 300.0f);
        if (nk_begin(ctx, "Debug", debug_wnd_rect, NK_WINDOW_BORDER | NK_WINDOW_TITLE))
        {
            nk_layout_row_dynamic(ctx, 30, 1);

            nk_checkbox_label(ctx, "Skip validation", &skip_validation);
            float ww           = nk_window_get_content_region(ctx).w;
            float button_width = 135.0f;
            float x_pos        = (ww - button_width) / 2;

            nk_layout_space_begin(ctx, NK_STATIC, 40, 1);
            nk_layout_space_push(ctx, nk_rect(x_pos, 0, button_width, 30));

            nk_layout_space_end(ctx);
            nk_layout_space_begin(ctx, NK_STATIC, 40, 1);
            nk_layout_space_push(ctx, nk_rect(x_pos, 0, button_width, 30));

            if (nk_button_label(ctx, "Test"))
            {
                Login::OpenModal("Testing here: <a href=\"https://google.com\">Test anchor</a>");
            }

            nk_layout_space_end(ctx);
        }
        nk_end(ctx);*/

#endif
        nk_d3d9_render(NK_ANTI_ALIASING_ON);
    }

    void InitNuklearCtx(IDirect3DDevice9* device)
    {
        D3DVIEWPORT9 viewport;
        if (SUCCEEDED(device->GetViewport(&viewport)))
        {
            ctx = nk_d3d9_init(device, viewport.Width, viewport.Height);
            nk_d3d9_font_stash_begin(&atlas);
            body_font   = nk_font_atlas_add_from_file(atlas, "eqnexus/Roboto-Regular.ttf", 17, 0);
            header_font = nk_font_atlas_add_from_file(atlas, "eqnexus/Raleway-Bold.ttf", 16, 0);
            nk_d3d9_font_stash_end();
            nk_style_load_all_cursors(ctx, atlas->cursors);
            nk_style_set_font(ctx, &body_font->handle);

            struct nk_style* s = &ctx->style;
            if (D3D9Hooks::BackgroundTexture())
            {
                struct nk_image tex                = nk_image_ptr(D3D9Hooks::BackgroundTexture());
                ctx->style.window.fixed_background = nk_style_item_image(tex);
            }

            auto text_color               = nk_rgb(240, 240, 240);
            s->text.color                 = text_color;
            s->window.border_color        = border;
            s->window.border              = 1.5f;
            s->window.header.label_normal = s->window.header.label_hover = s->window.header.label_active = nk_rgb(255, 218, 96);
            s->window.header.align                                                                       = NK_HEADER_RIGHT;
            s->window.header.normal = s->window.header.active = nk_style_item_color(nk_rgb(41, 56, 74));
            s->button.text_active                             = border;
            s->button.text_hover                              = nk_rgb(255, 218, 96);
            s->button.text_normal                             = text_color;
            s->button.normal                                  = nk_style_item_color(nk_rgb(41, 56, 74));
            s->button.hover                                   = nk_style_item_color(nk_rgb(30, 30, 50));
            s->button.active                                  = nk_style_item_color(nk_rgb(22, 22, 55));
            s->button.border_color                            = border;
            s->window.padding                                 = nk_vec2(10, 10);
            s->window.spacing                                 = nk_vec2(5, 5);

            nuklear_initialized = true;
            FetchServerData();
        }
    }

    void OnReset(IDirect3DDevice9* device)
    {
        auto state = GetGameState();
        if (nuklear_initialized)
        {
            nk_d3d9_shutdown();
            nuklear_initialized = false;
            if (state > -1)
            {
                return;
            }
        }
        InitNuklearCtx(device);
    }

    void PollDirectInputForNuklear()
    {
        if (mouse && SUCCEEDED(mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID) &mouse_state)))
        {
            if (POINT cursorPos; GetCursorPos(&cursorPos))
            {
                ScreenToClient(GetActiveWindow(), &cursorPos);
                mouse_x = cursorPos.x;
                mouse_y = cursorPos.y;
            }
            nk_input_motion(ctx, mouse_x, mouse_y);
            nk_input_button(ctx, NK_BUTTON_LEFT, mouse_x, mouse_y, (mouse_state.rgbButtons[0] & 0x80) != 0);
            nk_input_button(ctx, NK_BUTTON_RIGHT, mouse_x, mouse_y, (mouse_state.rgbButtons[1] & 0x80) != 0);
        }
    }

    int GetGameState()
    {
        return eqlib::pEverQuest ? eqlib::pEverQuest->GameState : -1;
    }
    bool DoLogin(int server_id)
    {
        for (auto& server : servers)
        {
            if (server->GetClientServer() && server->GetClientServer()->ID == server_id)
            {
                if (server->IsValidating())
                {
                    Login::InterceptUnknown(
                        "<b>Failed to join server<br></br>Your server files "
                        "are currently validating update status: " +
                        server->GetLongName() + ".");
                    Server::SetContext("");
                    return true;
                }

                server->ValidateInstallAsync(true);
#ifdef DEV
                if (!skip_validation && !server->UpToDate())
                {
#else
                if (!server->UpToDate())
                {
#endif
                    Login::InterceptUnknown(
                        "<b>Failed to join custom server<br></br>Your patch files are out "
                        "of date for " +
                        server->GetLongName() + ".");
                    Server::SetContext("");
                    return true;
                }
                Server::SetContext(server->GetShortName());
                auto [prevented, reason] = Config::PreventLogin();
                if (prevented)
                {
                    Login::InterceptUnknown(reason.c_str());
                    Server::SetContext("");
                    return true;
                }
                return false;
            }
        }
        Server::SetContext("");
        return false;
    }

    void ShowPopup(const std::string& msg)
    {
        Login::OpenModal(msg);
    }
};
