import globals;
import overlay;
import d3d9_hooks;

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

bool SetupCore() {
    if (MH_Initialize() != MH_OK) {
        std::cerr << "Failed to initialize MinHook." << std::endl;
        return false;
    }
    //WaitForDebugger();
    D3D9Hooks::Init();
    eqoverlay = std::make_unique<EQOverlay>();
    eqlib::InitializeGlobals();
    return true;
}

bool did_init = false;

extern "C" __declspec(dllexport) bool Initialize(IDirectInputDevice8A* keyboard, IDirectInputDevice8A* mouse) {
#ifdef _DEBUG
    AttachConsoleToDLL();
#endif
    if (!did_init) {
        SetupCore();
        did_init = true;
    }
    eqoverlay->SetKeyboard(keyboard);
    eqoverlay->SetMouse(mouse);
    return true;
}

extern "C" __declspec(dllexport) bool Teardown() {
    if (!did_init) {
        return true;
    }
    eqoverlay->Reset();
    eqoverlay = nullptr;
    eqlib::CleanupEQMainOffsets();
    D3D9Hooks::Teardown();
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

