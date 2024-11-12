// d3d9_hooks.ixx

export module d3d9_hooks;

import <d3d9.h>;
import <functional>;
import <iostream>;
import <string>;
import <MinHook.h>;

using DeviceCallback = std::function<void(IDirect3DDevice9*)>;

export class D3D9Hooks
{
   public:
    static bool Init()
    {
        if (auto hD3D9 = GetModuleHandle(L"d3d9.dll"); hD3D9)
        {
            pDirect3DCreate9 = GetProcAddress(hD3D9, "Direct3DCreate9");
            if (!pDirect3DCreate9)
            {
                std::cerr << "Failed to get Direct3DCreate9 function address." << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "d3d9.dll is not loaded." << std::endl;
            return false;
        }

        using Direct3DCreate9_t = IDirect3D9*(WINAPI*) (UINT);
        auto Direct3DCreate9    = reinterpret_cast<Direct3DCreate9_t>(pDirect3DCreate9);
        if (auto pD3D9 = Direct3DCreate9(D3D_SDK_VERSION); pD3D9)
        {
            HWND hwnd = CreateWindowEx(0,
                                       L"STATIC",
                                       nullptr,
                                       WS_OVERLAPPEDWINDOW,
                                       0,
                                       0,
                                       100,
                                       100,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       nullptr);

            D3DPRESENT_PARAMETERS d3dpp{};
            d3dpp.Windowed      = TRUE;
            d3dpp.SwapEffect    = D3DSWAPEFFECT_DISCARD;
            d3dpp.hDeviceWindow = hwnd;

            IDirect3DDevice9* pDummyDevice = nullptr;
            if (HRESULT hr = pD3D9->CreateDevice(D3DADAPTER_DEFAULT,
                                                 D3DDEVTYPE_HAL,
                                                 hwnd,
                                                 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                                 &d3dpp,
                                                 &pDummyDevice);
                SUCCEEDED(hr) && pDummyDevice)
            {
                auto vtable = *reinterpret_cast<void***>(pDummyDevice);

                if (!CreateHook(vtable[42], &D3D9Hooks::HookedBeginScene, original_beginscene) ||
                    !CreateHook(vtable[16], &D3D9Hooks::HookedReset, original_reset))
                {
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
            else
            {
                std::cerr << "Failed to create dummy IDirect3DDevice9 instance." << std::endl;
                pD3D9->Release();
                DestroyWindow(hwnd);
                return false;
            }
        }
        else
        {
            std::cerr << "Failed to create IDirect3D9 instance." << std::endl;
            return false;
        }
    }

    static void SetRenderCallback(DeviceCallback cb)
    {
        render_callback = cb;
    }
    static void ClearRenderCallback()
    {
        render_callback = nullptr;
    }
    static void SetResetCallback(DeviceCallback cb)
    {
        reset_callback = cb;
    }
    static void ClearResetCallback()
    {
        reset_callback = nullptr;
    }

    static void Teardown()
    {
        DisableHook(original_beginscene);
        DisableHook(original_reset);

        render_callback = nullptr;
        reset_callback  = nullptr;

        std::cout << "D3D9 hooks and resources have been successfully torn down." << std::endl;
    }

   private:
    using Reset_t      = HRESULT(WINAPI*)(IDirect3DDevice9* device,
                                     D3DPRESENT_PARAMETERS* pPresentationParameters);
    using BeginScene_t = HRESULT(WINAPI*)(IDirect3DDevice9* device);

    static bool CreateHook(void* target, void* detour, auto& original)
    {
        if (MH_CreateHook(target, detour, reinterpret_cast<void**>(&original)) == MH_OK)
        {
            MH_EnableHook(target);
            return true;
        }
        else
        {
            std::cerr << "Failed to create hook." << std::endl;
            return false;
        }
    }

    static void DisableHook(auto& original)
    {
        if (original)
        {
            MH_DisableHook(reinterpret_cast<void*>(original));
            MH_RemoveHook(reinterpret_cast<void*>(original));
        }
    }

    static HRESULT WINAPI HookedBeginScene(IDirect3DDevice9* device)
    {
        if (!original_beginscene)
            return 0;

        if (render_callback)
            render_callback(device);
        return original_beginscene(device);
    }

    static HRESULT WINAPI HookedReset(IDirect3DDevice9* device,
                                      D3DPRESENT_PARAMETERS* pPresentationParameters)
    {
        if (!original_reset)
            return 0;

        HRESULT hr = original_reset(device, pPresentationParameters);
        if (SUCCEEDED(hr) && reset_callback)
            reset_callback(device);
        return hr;
    }

    inline static FARPROC pDirect3DCreate9         = nullptr;
    inline static IDirect3DDevice9* pD3DDevice     = nullptr;
    inline static Reset_t original_reset           = nullptr;
    inline static BeginScene_t original_beginscene = nullptr;
    inline static DeviceCallback render_callback   = nullptr;
    inline static DeviceCallback reset_callback    = nullptr;
};
