
import core;
#ifdef DEV
import debug;
#endif
#include <Windows.h>
#include <cstdio>
#include <csignal>
#include <string>
#include <map>
#include <iostream>
#include <dinput.h>
#include <d3d9.h>
#include "MinHook.h"


typedef HRESULT(WINAPI* CreateInputDevice_t)(IDirectInput8A*, REFGUID, LPDIRECTINPUTDEVICE8A*, LPUNKNOWN);
IDirectInputDevice8A* pKeyboardDevice = nullptr;
IDirectInputDevice8A* pMouseDevice = nullptr;
CreateInputDevice_t originalCreateInputDevice = nullptr;
inline const GUID MyGUID_SysMouse = { 0x6F1D2B60, 0xD5A0, 0x11CF, { 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
inline const GUID MyGUID_SysKeyboard = { 0x6F1D2B61, 0xD5A0, 0x11CF, { 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
HMODULE hOriginalDll = NULL;
std::map<std::string, FARPROC> functionCache;

std::wstring ConvertToWide(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    wstr.pop_back();
    return wstr;
}

std::string ConvertToNarrow(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    str.pop_back();
    return str;
}

std::string FindOriginalDinput8() {
    wchar_t systemDir[MAX_PATH];
    UINT length = GetSystemWow64Directory(systemDir, MAX_PATH);

    if (length == 0 || length >= MAX_PATH) {
        return "";
    }

    return ConvertToNarrow(std::wstring(systemDir) + L"\\dinput8.dll");
}

void LoadOriginalDInput8() {
    if (!hOriginalDll) {
        std::string originalDInput8Path = FindOriginalDinput8();
        hOriginalDll = LoadLibrary(ConvertToWide(originalDInput8Path).c_str());
        if (!hOriginalDll) {
            MessageBox(NULL, L"Failed to load original dinput8.dll!", L"Error", MB_OK);
            ExitProcess(EXIT_FAILURE);
        }
    }
}

FARPROC GetOriginalFunction(const char* funcName) {
    LoadOriginalDInput8();

    if (auto it = functionCache.find(funcName); it != functionCache.end()) {
        return it->second;
    }

    FARPROC proc = GetProcAddress(hOriginalDll, funcName);
    if (proc) {
        functionCache[funcName] = proc;
    }
    else {
        MessageBox(NULL, ConvertToWide("Failed to resolve function: " + std::string(funcName)).c_str(), L"Error", MB_OK);
    }

    return proc;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    {
        if (MH_Initialize() != MH_OK) {
            return FALSE;
        }
        LoadCore();
        break;
    }
    case DLL_PROCESS_DETACH:
        if (hOriginalDll) {
            FreeLibrary(hOriginalDll);
        }
        MH_Uninitialize();
        break;
    }
    return TRUE;
}

HRESULT WINAPI HookedCreateInputDevice(IDirectInput8A* pDirectInput, REFGUID rguid, LPDIRECTINPUTDEVICE8A* lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
    HRESULT hr = originalCreateInputDevice(pDirectInput, rguid, lplpDirectInputDevice, pUnkOuter);

    if (SUCCEEDED(hr) && lplpDirectInputDevice) {
        if (rguid == MyGUID_SysKeyboard) {
            SetKeyboardDevice(*lplpDirectInputDevice);
        }
        else if (rguid == MyGUID_SysMouse) {
            SetMouseDevice(*lplpDirectInputDevice);
        }
    }
    if (GetKeyboardDevice() && GetMouseDevice()) {
#ifdef DEV
        StartDebugPoll();
        std::signal(SIGINT, SignalHandler);
        std::atexit(StopDebugPoll);
#endif
        LoadDevices();
    }

    return hr;
}

// Proxy functions
extern "C" __declspec(dllexport) HRESULT WINAPI Proxy_DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter) {
    auto DirectInput8CreateFunc = reinterpret_cast<HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN)>(
        GetOriginalFunction("DirectInput8Create"));

    HRESULT hr = DirectInput8CreateFunc(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    if (FAILED(hr)) return hr;

    IDirectInput8A* pDirectInput = static_cast<IDirectInput8A*>(*ppvOut);

    void** vtable = *reinterpret_cast<void***>(pDirectInput);
    if (MH_CreateHook(vtable[3], HookedCreateInputDevice, reinterpret_cast<void**>(&originalCreateInputDevice)) == MH_OK) {
        MH_EnableHook(vtable[3]);
    }
    return hr;
}


extern "C" __declspec(dllexport) HRESULT WINAPI Proxy_DllCanUnloadNow() {
    return reinterpret_cast<HRESULT(WINAPI*)()>(GetOriginalFunction("DllCanUnloadNow"))();
}

extern "C" __declspec(dllexport) HRESULT WINAPI Proxy_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppv) {
    return reinterpret_cast<HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*)>(GetOriginalFunction("DllGetClassObject"))(rclsid, riid, ppv);
}

extern "C" __declspec(dllexport) HRESULT WINAPI Proxy_DllRegisterServer() {
    return reinterpret_cast<HRESULT(WINAPI*)()>(GetOriginalFunction("DllRegisterServer"))();
}

extern "C" __declspec(dllexport) HRESULT WINAPI Proxy_DllUnregisterServer() {
    return reinterpret_cast<HRESULT(WINAPI*)()>(GetOriginalFunction("DllUnregisterServer"))();
}

extern "C" __declspec(dllexport) LPCDIDATAFORMAT WINAPI Proxy_GetdfDIJoystick() {
    return reinterpret_cast<LPCDIDATAFORMAT(WINAPI*)()>(GetOriginalFunction("GetdfDIJoystick"))();
}
