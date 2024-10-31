#include "pch.h"
#include "d3d9_hooks.h"
#include <iostream>
#include <string>

DeviceCallback D3D9Hooks::render_callback = nullptr;
DeviceCallback D3D9Hooks::reset_callback = nullptr;
FARPROC D3D9Hooks::pDirect3DCreate9 = nullptr;
IDirect3DDevice9* D3D9Hooks::pD3DDevice = nullptr;
D3D9Hooks::Reset_t D3D9Hooks::original_reset = nullptr;
D3D9Hooks::BeginScene_t D3D9Hooks::original_beginscene = nullptr;

bool D3D9Hooks::Init() {
    HMODULE hD3D9 = GetModuleHandle(L"d3d9.dll");
    if (!hD3D9) {
        std::cerr << "d3d9.dll is not loaded." << std::endl;
        return false;
    }

    pDirect3DCreate9 = GetProcAddress(hD3D9, "Direct3DCreate9");
    if (!pDirect3DCreate9) {
        std::cerr << "Failed to get Direct3DCreate9 function address." << std::endl;
        return false;
    }

    typedef IDirect3D9* (WINAPI* Direct3DCreate9_t)(UINT);
    Direct3DCreate9_t Direct3DCreate9 = (Direct3DCreate9_t)pDirect3DCreate9;
    IDirect3D9* pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D9) {
        std::cerr << "Failed to create IDirect3D9 instance." << std::endl;
        return false;
    }

    HWND hwnd = CreateWindowEx(0, L"STATIC", NULL, WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, NULL, NULL);

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hwnd;

    IDirect3DDevice9* pDummyDevice = nullptr;
    HRESULT hr = pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp, &pDummyDevice);
    if (FAILED(hr) || !pDummyDevice) {
        std::cerr << "Failed to create dummy IDirect3DDevice9 instance." << std::endl;
        pD3D9->Release();
        DestroyWindow(hwnd);
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(pDummyDevice);

    if (MH_CreateHook(vtable[41], &D3D9Hooks::HookedBeginScene, reinterpret_cast<void**>(&original_beginscene)) == MH_OK) {
        MH_EnableHook(vtable[41]);
    }
    else {
        std::cerr << "Failed to create hook for BeginScene." << std::endl;
        pDummyDevice->Release();
        pD3D9->Release();
        DestroyWindow(hwnd);
        return false;
    }

    if (MH_CreateHook(vtable[16], &D3D9Hooks::HookedReset, reinterpret_cast<void**>(&original_reset)) == MH_OK) {
        MH_EnableHook(vtable[16]);
    }
    else {
        std::cerr << "Failed to create hook for Reset." << std::endl;
        pDummyDevice->Release();
        pD3D9->Release();
        DestroyWindow(hwnd);
        return false;
    }

    pD3DDevice = pDummyDevice;
    pD3D9->Release();
    DestroyWindow(hwnd);

    return true;
}

HRESULT WINAPI D3D9Hooks::HookedBeginScene(IDirect3DDevice9* device) {
    if (!original_beginscene) {
        return 0;
    }
    auto result= original_beginscene(device);
    if (render_callback) {
        render_callback(device);
    }
    return result;
}

HRESULT WINAPI D3D9Hooks::HookedReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters) {
    if (!original_reset) {
        return 0;
    }
    HRESULT hr = original_reset(device, pPresentationParameters);

    if (SUCCEEDED(hr) && reset_callback) {
        reset_callback(device);
    }

    return hr;
}


void D3D9Hooks::Teardown() {
    if (original_beginscene) {
        MH_DisableHook(reinterpret_cast<void*>(original_beginscene));
        MH_RemoveHook(reinterpret_cast<void*>(original_beginscene));
        original_beginscene = nullptr;
    }

    if (original_reset) {
        MH_DisableHook(reinterpret_cast<void*>(original_reset));
        MH_RemoveHook(reinterpret_cast<void*>(original_reset));
        original_reset = nullptr;
    }
    // Reset any other static variables or callbacks to nullptr
    render_callback = nullptr;
    reset_callback = nullptr;
    pDirect3DCreate9 = nullptr;

    std::cout << "D3D9 hooks and resources have been successfully torn down." << std::endl;
}
