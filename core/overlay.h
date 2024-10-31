
#pragma once
#include "pch.h"
#include <windows.h>
#include <dinput.h>
#include <string>
#include <functional>
#include "eqmodule.h"
#include "nuklear_d3d9.h"
#include "d3d9_hooks.h"
#include "eq/globals.h"


struct ServerInfo {
    std::string shortname = "";
    std::string longname = "";
    std::string url = "";
};

class EQOverlay : public EQModule {
public:
    EQOverlay() {
        D3D9Hooks::SetRenderCallback([&](IDirect3DDevice9* device) {
            this->OnRender(device);
        });
        D3D9Hooks::SetResetCallback([&](IDirect3DDevice9* device) {
            this->OnReset(device);
        });
        FetchServerData();
    };
    void SetKeyboard(IDirectInputDevice8A* keyboard) { this->keyboard = keyboard; }
    void SetMouse(IDirectInputDevice8A* mouse) { this->mouse = mouse; }
    void Reset() override {
        D3D9Hooks::ClearRenderCallback();
        D3D9Hooks::ClearResetCallback();
        nk_free(ctx);
    }

private:
    void InitializeBounds();
    void FetchServerData();
    void OnRender(IDirect3DDevice9* device);
    void OnReset(IDirect3DDevice9* device);
    void PollDirectInputForNuklear();
    void InitNuklearCtx(IDirect3DDevice9* device);
    int GetGameState();
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
};
