// d3d9_hooks.ixx

export module d3d9_hooks;

import <Windows.h>;
import <d3d9.h>;
import <functional>;
import <iostream>;
import <string>;
import <MinHook.h>;

#include <stb_image.h>

using DeviceCallback = std::function<void(IDirect3DDevice9*)>;

export class D3D9Hooks
{
   public:
    static bool Init()
    {
        if (initialized)
        {
            return true;
        }
        initialized = true;
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
            HWND hwnd = CreateWindowEx(0, L"STATIC", nullptr, WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, nullptr, nullptr);

            D3DPRESENT_PARAMETERS d3dpp{};
            d3dpp.Windowed      = TRUE;
            d3dpp.SwapEffect    = D3DSWAPEFFECT_DISCARD;
            d3dpp.hDeviceWindow = hwnd;

            IDirect3DDevice9* pDummyDevice = nullptr;
            if (HRESULT hr = pD3D9->CreateDevice(
                    D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);
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

        std::cout << "D3D9 hooks and resources have been successfully torn down." << std::endl;
    }

    static IDirect3DTexture9* const BackgroundTexture()
    {
        return bg;
    }

    static IDirect3DTexture9* const SeparatorTexture()
    {
        return sep;
    }

    static IDirect3DTexture9* LoadTexture(LPDIRECT3DDEVICE9 d3dDevice, const char* filename, float opacity = 1.0f)
    {
        // Clamp opacity to [0.0, 1.0]
        if (opacity < 0.0f)
            opacity = 0.0f;
        if (opacity > 1.0f)
            opacity = 1.0f;

        // Load image with stb_image
        int width, height, channels;
        unsigned char* image = stbi_load(filename, &width, &height, &channels, 4);  // Force 4 channels (RGBA)
        if (!image)
        {
            printf("Failed to load image: %s\n", stbi_failure_reason());
            return nullptr;
        }

        // Create a Direct3D texture
        IDirect3DTexture9* texture = nullptr;
        HRESULT hr                 = d3dDevice->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, nullptr);

        if (FAILED(hr))
        {
            printf("Failed to create Direct3D texture. HRESULT: 0x%08X\n", hr);
            stbi_image_free(image);
            return nullptr;
        }

        // Modify the alpha channel to apply opacity
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                unsigned char* pixel = image + (y * width + x) * 4;
                pixel[3]             = static_cast<unsigned char>(pixel[3] * opacity);  // Scale alpha by opacity
            }
        }

        // Lock the texture to copy pixel data
        D3DLOCKED_RECT lockedRect;
        hr = texture->LockRect(0, &lockedRect, nullptr, 0);
        if (FAILED(hr))
        {
            printf("Failed to lock texture. HRESULT: 0x%08X\n", hr);
            texture->Release();
            stbi_image_free(image);
            return nullptr;
        }

        // Copy pixel data into the texture, converting RGBA to ARGB
        unsigned char* dest = (unsigned char*) lockedRect.pBits;
        for (int y = 0; y < height; ++y)
        {
            unsigned char* srcRow  = image + y * width * 4;        // Source row in RGBA format
            unsigned char* destRow = dest + y * lockedRect.Pitch;  // Destination row in ARGB format

            for (int x = 0; x < width; ++x)
            {
                unsigned char r = srcRow[x * 4 + 0];  // Red
                unsigned char g = srcRow[x * 4 + 1];  // Green
                unsigned char b = srcRow[x * 4 + 2];  // Blue
                unsigned char a = srcRow[x * 4 + 3];  // Alpha

                destRow[x * 4 + 0] = b;  // Blue
                destRow[x * 4 + 1] = g;  // Green
                destRow[x * 4 + 2] = r;  // Red
                destRow[x * 4 + 3] = a;  // Alpha
            }
        }

        texture->UnlockRect(0);

        // Free the image data
        stbi_image_free(image);

        return texture;
    }

   private:
    using Reset_t      = HRESULT(WINAPI*)(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
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
        bg          = nullptr;
        sep         = nullptr;
        initialized = false;
    }

    static HRESULT WINAPI HookedBeginScene(IDirect3DDevice9* device)
    {
        if (!bg)
            bg  = LoadTexture(device, "eqnexus/bg.png", 0.9f);
        if (!sep)
            sep = LoadTexture(device, "eqnexus/sep.png", 0.86f);
        
        if (!original_beginscene)
            return 0;

        if (render_callback)
            render_callback(device);
        return original_beginscene(device);
    }

    static HRESULT WINAPI HookedReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
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
    inline static IDirect3DTexture9* bg            = nullptr;
    inline static IDirect3DTexture9* sep           = nullptr;
    inline static bool initialized                 = false;
};
