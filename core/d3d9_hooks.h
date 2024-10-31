
#pragma once
#include "pch.h"
#include <functional>
#include <d3d9.h>
#include <string>
#include "MinHook.h"

using DeviceCallback = std::function<void(IDirect3DDevice9*)>;

class D3D9Hooks {
public:
    static bool Init();
    static void SetRenderCallback(DeviceCallback cb) { render_callback = cb; };
    static void ClearRenderCallback() { render_callback = nullptr; };
    static void SetResetCallback(DeviceCallback cb) { reset_callback = cb; };
    static void ClearResetCallback() { reset_callback = nullptr; };
    static void Teardown();
private:
    typedef HRESULT(WINAPI* Reset_t)(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
    typedef HRESULT(WINAPI* BeginScene_t)(IDirect3DDevice9* device);
    
    static HRESULT WINAPI HookedBeginScene(IDirect3DDevice9* device);
    static HRESULT WINAPI HookedReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);

    static FARPROC pDirect3DCreate9;
    static IDirect3DDevice9* pD3DDevice;
    static Reset_t original_reset;
    static BeginScene_t original_beginscene;
    static DeviceCallback render_callback;
    static DeviceCallback reset_callback;
};
