import globals;
import overlay;
import d3d9_hooks;
import hooks;
import filesystem;
import login;
import config;
import server;

#include <MinHook.h>
#include <iostream>
#include <cstdio>
#include <vector>
#include <dinput.h>

std::unique_ptr<EQOverlay> eqoverlay = nullptr;

void AttachConsoleToDLL() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    std::cout.clear();
}

void WaitForDebugger() {
    std::cout << "Waiting for debugger to attach..." << std::endl;
    while (!IsDebuggerPresent()) {
        Sleep(100); // Wait 100 milliseconds before checking again
    }
    std::cout << "Debugger attached." << std::endl;
}

IDirectInputDevice8A* keyboard = nullptr;
IDirectInputDevice8A* mouse = nullptr;
bool SetupCore() {
    if (MH_Initialize() != MH_OK) {
        std::cerr << "Failed to initialize MinHook." << std::endl;
        return false;
    }

    Hooks::Init([]() {
        D3D9Hooks::Init();
        Login::Init();
        Server::Init();
        FileSystem::Init();
        Config::Init();
        eqoverlay = std::make_unique<EQOverlay>();
        if (mouse && keyboard) {
            eqoverlay->SetKeyboard(keyboard);
            eqoverlay->SetMouse(mouse);
        }
    });
   
    return true;
}

bool did_init = false;

extern "C" __declspec(dllexport) bool SetDevices(IDirectInputDevice8A* k, IDirectInputDevice8A* m) { 
    keyboard = k;
    mouse = m;
    if (eqoverlay) {
        std::cout << "Settings devices for keyboard and mouse" << std::endl;
        eqoverlay->SetKeyboard(k);
        eqoverlay->SetMouse(m);
    }
    else {
        std::cout << "Overlay not initialized in SetDevices" << std::endl;
    }
    return true;
}

extern "C" __declspec(dllexport) bool Initialize() {
    if (!did_init) {
#ifdef DEV
        AttachConsoleToDLL();
#endif
        SetupCore();
        did_init = true;
    }

    return true;
}

extern "C" __declspec(dllexport) bool Teardown() {
    if (!did_init) {
        return true;
    }
    D3D9Hooks::Teardown();
    eqoverlay.reset();
    did_init = false;
    Server::Teardown();
    Hooks::Teardown();
    Login::Teardown();
    FileSystem::Teardown();
    Config::Teardown();
   

    if (MH_Uninitialize() != MH_OK) {
        std::cerr << "Failed to uninitialize MinHook." << std::endl;
        return false;
    }
    return true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    {
        break;
    }
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

