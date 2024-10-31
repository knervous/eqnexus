#include "pch.h"
#include <windows.h>
#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include <dinput.h>
#include <math.h>
#include <array>
#include <initializer_list>
#include <d3d9.h>

#include "MinHook.h"

typedef char* (__fastcall* AddZone_t)(int*, char*, unsigned __int32, unsigned __int32, char*, char*, unsigned __int32, __int32, __int32, __int32, __int32);
typedef int(__fastcall* LoadEQStr_t)(DWORD*, char*, const char*, char);
typedef char* (__fastcall* GetEQStr_t)(DWORD*, char*, DWORD, bool*);
typedef int(__fastcall* GetEQItem_t)(int*, char*, unsigned int, int, int);
typedef unsigned int** (__fastcall* StringTable_AddItem_t)(DWORD*, char*, unsigned int*);
typedef void(__fastcall* ReceivePacket_t)(DWORD* pthis, char* pChar, void* data, int size, int a4, int* portPtr);
typedef void(__fastcall* SendPacket_t)(DWORD* pthis, char* pChar, int a2, void* a3, size_t size);
typedef void(__fastcall* LoginProcessPacket)(void* pthis, char* pChar, uint16_t* a2, void* a3);

DWORD baseAddress = (DWORD)GetModuleHandle(NULL);

auto pinstStringTable = (((DWORD)0xDD25C4 - 0x400000) + baseAddress);
auto AddZone = (((DWORD)0x007DC430 - 0x400000) + baseAddress);
auto StringTable_GetString = (((DWORD)0x007D0660 - 0x400000) + baseAddress);
auto StringTable_Load = (((DWORD)0x007D06C0 - 0x400000) + baseAddress);
auto StringTable_AddItem = (((DWORD)0x007D0540 - 0x400000) + baseAddress);
auto StringTable_GetItem = (((DWORD)0x007D04F0 - 0x400000) + baseAddress);
// auto Client_ReceivePacket = (((DWORD)0xB770D0 - 0x6B0000) + baseAddress);
auto Client_ReceivePacket = (((DWORD)0xB7A460 - 0x6B0000) + baseAddress);
auto Client_SendPacket = (((DWORD)0xB751F0 - 0x6B0000) + baseAddress);

// Send message
// char __thiscall sub_B751F0(int this, int a2, _BYTE *a3, size_t Size)

AddZone_t OriginalAddZone = nullptr;
LoadEQStr_t OriginalStringTable_Load = nullptr;
GetEQItem_t OriginalStringTable_GetItem = nullptr;
StringTable_AddItem_t OriginalStringTable_AddItem = nullptr;
ReceivePacket_t OriginalReceivePacket = nullptr;
SendPacket_t OriginalSendPacket = nullptr;
GetEQStr_t GetEQStr = reinterpret_cast<GetEQStr_t>(StringTable_GetString);

struct EQItem {
    DWORD id;
    const char* name;
};

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

// Store the last max ID from adding an item so we can increment and create a new id in memory
DWORD max_id = 0;

unsigned int** __fastcall DetourStringTable_AddItem(DWORD* pThis, char* pPtr, unsigned int* a2) {
    auto item = reinterpret_cast<EQItem*>(a2);
    max_id = max_id > item-> id ? max_id : item->id;
    return OriginalStringTable_AddItem(pThis, pPtr, a2);
}

char* __fastcall DetourAddZone(int* pThis, char* pPtr, unsigned __int32 zoneType, unsigned __int32 zoneID, char* zoneShortName, char* zoneLongName, unsigned __int32 eqStrID, __int32 zoneFlags2, __int32 x, __int32 y, __int32 z) {
    //auto* itemEntry = new EQItem{ max_id + 1, "Testing String Here" };
    //DetourStringTable_AddItem((DWORD*)pinstStringTable, nullptr, (unsigned int*)itemEntry);
    //bool success = false;
    //auto addedEntry = GetEQStr((DWORD*)pinstStringTable, nullptr, itemEntry->id, &success);
    return OriginalAddZone(pThis, pPtr, zoneType, zoneID, zoneShortName, zoneLongName, eqStrID, zoneFlags2, x, y, z);
}

int __fastcall DetourLoadEQStr(DWORD* pThis, char* pPtr, const char* a2, char a3) {
    return OriginalStringTable_Load(pThis, pPtr, a2, a3);
}

void __fastcall DetourReceivePacket(DWORD* pthis, char* pChar, void* data, int size, int a4, int* portPtr) {

    OriginalReceivePacket(pthis, pChar, data, size, a4, portPtr);

    return;
}

#pragma pack(1)

struct LoginInfo_Struct {
    /*000*/	char	login_info[64];
    /*064*/	char	custom_header[124];
    /*188*/	uint8_t	zoning;			// 01 if zoning, 00 if not
    /*189*/	uint8_t	unknown189[275];
    /*488*/
};

const uint16_t OP_SendLoginInfo = 0x7a09;


void __fastcall DetourSendPacket(DWORD* pthis, char* pChar, int a2, void* data, size_t size) {
    unsigned char* byteData = reinterpret_cast<unsigned char*>(data);
    uint16_t opcode = (static_cast<uint16_t>(byteData[1]) << 8) | static_cast<uint16_t>(byteData[0]);
    switch (opcode) {
    case OP_SendLoginInfo:
    {
        LoginInfo_Struct* li = reinterpret_cast<LoginInfo_Struct*>(byteData + 2);
        strncpy_s(li->custom_header, sizeof(li->custom_header), "eqcustom", _TRUNCATE);
        break;
    }
    default:
        break;
    }
    OriginalSendPacket(pthis, pChar, a2, data, size);
    return;
}

typedef void(__fastcall* SendPacket_t)(DWORD* pthis, char* pChar, int a2, void* a3, size_t size);


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    {
        if (MH_Initialize() != MH_OK) {
            return FALSE;
        }
        
        std::initializer_list<std::tuple<LPVOID, LPVOID, LPVOID*>> hooks = {
            { (LPVOID)AddZone, &DetourAddZone, reinterpret_cast<LPVOID*>(&OriginalAddZone) },
            { (LPVOID)StringTable_Load, &DetourLoadEQStr, reinterpret_cast<LPVOID*>(&OriginalStringTable_Load) },
            { (LPVOID)Client_ReceivePacket, &DetourReceivePacket, reinterpret_cast<LPVOID*>(&OriginalReceivePacket) },
            { (LPVOID)Client_SendPacket, &DetourSendPacket, reinterpret_cast<LPVOID*>(&OriginalSendPacket)},
            { (LPVOID)StringTable_AddItem, &DetourStringTable_AddItem, reinterpret_cast<LPVOID*>(&OriginalStringTable_AddItem) }
        };

        for (const auto& [target, detour, original] : hooks) {
            if (MH_CreateHook(target, detour, original) != MH_OK) {
                return FALSE;
            }

            if (MH_EnableHook(target) != MH_OK) {
                return FALSE;
            }
        }

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

typedef bool (*CoreInitialize_t)(void* keyboard, void* mouse);
typedef bool (*CoreTeardown_t)();

typedef HRESULT(WINAPI* CreateInputDevice_t)(IDirectInput8A*, REFGUID, LPDIRECTINPUTDEVICE8A*, LPUNKNOWN);

IDirectInputDevice8A* pKeyboardDevice = nullptr;
IDirectInputDevice8A* pMouseDevice = nullptr;
inline const GUID MyGUID_SysMouse = { 0x6F1D2B60, 0xD5A0, 0x11CF, { 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
inline const GUID MyGUID_SysKeyboard = { 0x6F1D2B61, 0xD5A0, 0x11CF, { 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
CreateInputDevice_t originalCreateInputDevice = nullptr;

bool core_loaded = false;

void LoadCore() {
    auto lib = LoadLibrary(L"eqnexus/core.dll");
    if (lib) {
        CoreInitialize_t Initialize = (CoreInitialize_t)GetProcAddress(lib, "Initialize");
        if (Initialize) {
            bool success = Initialize(pKeyboardDevice, pMouseDevice);
            if (success) {
                std::cout << "Initialization successful!" << std::endl;
                core_loaded = true;
            }
            else {
                std::cout << "Initialization failed." << std::endl;
            }
        }
        else {
            MessageBox(NULL, L"Initialize function not found in core.dll", L"Missing Function", MB_OK);
        }

    }
    else {
        MessageBox(NULL, L"Nexus Core not found in eqnexus/core.dll", L"Missing files", MB_OK);
    }
}

void UnloadCore() {
    auto lib = GetModuleHandle(L"eqnexus/core.dll");
    if (lib) {
        CoreTeardown_t Teardown = (CoreTeardown_t)GetProcAddress(lib, "Teardown");
        if (Teardown) {
            bool success = Teardown();
            if (success) {
                std::cout << "Teardown successful!" << std::endl;
                FreeLibrary(lib);
                core_loaded = false;
            }
            else {
                std::cout << "Teardown failed." << std::endl;
            }
        }
        else {
            MessageBox(NULL, L"Teardown function not found in core.dll", L"Missing Function", MB_OK);
        }
    }
}

#ifdef _DEBUG
#include <thread>
#include <atomic>
#include <csignal>
#include <cstdlib>
bool ctrlRTriggered = false;
bool ctrlETriggered = false;
std::thread debugPollThread;
std::atomic<bool> keepPolling{ false };
void PollKeyboard() {
    while (keepPolling) {
        if (!pKeyboardDevice) continue;

        BYTE keyboardState[256];
        HRESULT hr = pKeyboardDevice->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
        if (FAILED(hr)) {
            // Re-acquire the device if needed
            if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
                pKeyboardDevice->Acquire();
            }
            continue;
        }

        // Check if CTRL is pressed
        bool ctrlPressed = (keyboardState[DIK_LCONTROL] & 0x80) || (keyboardState[DIK_RCONTROL] & 0x80);
        // Check for R and E keys
        bool rPressed = (keyboardState[DIK_R] & 0x80);
        bool ePressed = (keyboardState[DIK_E] & 0x80);

        // Trigger CTRL + R only once until reset
        if (ctrlPressed && rPressed && !ctrlRTriggered && !core_loaded) {
            std::cout << "Reloading module..." << std::endl;
            ctrlRTriggered = true;
            LoadCore();
            // Place handler for CTRL + R here
        }
        // Trigger CTRL + E only once until reset
        if (ctrlPressed && ePressed && !ctrlETriggered && core_loaded) {
            std::cout << "Unloading module..." << std::endl;
            ctrlETriggered = true;
            UnloadCore();
            // Place handler for CTRL + E here
        }

        // Reset trigger flags if any key in the combination is released
        if (!ctrlPressed || !rPressed) {
            ctrlRTriggered = false;
        }
        if (!ctrlPressed || !ePressed) {
            ctrlETriggered = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Polling interval
    }
}

void StartDebugPoll() {
    if (!keepPolling.exchange(true)) { // Start polling only if not already started
        debugPollThread = std::thread(PollKeyboard);
    }
}

void StopDebugPoll() {
    if (keepPolling.exchange(false)) {
        if (debugPollThread.joinable()) {
            debugPollThread.join();
        }
    }
}
void SignalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "SIGINT received. Stopping debug poll..." << std::endl;
        StopDebugPoll();
    }
}

#endif

HRESULT WINAPI HookedCreateInputDevice(IDirectInput8A* pDirectInput, REFGUID rguid, LPDIRECTINPUTDEVICE8A* lplpDirectInputDevice, LPUNKNOWN pUnkOuter) {
    // Call the original CreateDevice
    HRESULT hr = originalCreateInputDevice(pDirectInput, rguid, lplpDirectInputDevice, pUnkOuter);

    if (SUCCEEDED(hr) && lplpDirectInputDevice) {
        // Check if the created device is the keyboard or mouse and store the pointer
        if (rguid == MyGUID_SysKeyboard) {
            pKeyboardDevice = *lplpDirectInputDevice;
        }
        else if (rguid == MyGUID_SysMouse) {
            pMouseDevice = *lplpDirectInputDevice;
        }
    }
    if (pKeyboardDevice && pMouseDevice) {
        // Hot reloading for quick workflow
#ifdef _DEBUG
        StartDebugPoll();
        std::signal(SIGINT, SignalHandler);
        std::atexit(StopDebugPoll); // Ensures StopDebugPoll is called on exit
#endif
        LoadCore();
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
