import globals;
import overlay;
import d3d9_hooks;
import hooks;
import filesystem;
import login;
import config;
import server;
import updater;

#include <MinHook.h>
#include <dinput.h>

#include <cstdio>
#include <iostream>
#include <vector>
#include <tlhelp32.h>
#include <Psapi.h>

std::unique_ptr<EQOverlay> eqoverlay = nullptr;

void
AttachConsoleToDLL()
{
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    std::cout.clear();
}

IDirectInputDevice8A* keyboard = nullptr;
IDirectInputDevice8A* mouse    = nullptr;
bool
SetupCore()
{
    if (MH_Initialize() != MH_OK)
    {
        std::cerr << "Failed to initialize MinHook." << std::endl;
        return false;
    }

    Hooks::Init([]() {
        Login::Init();
        Server::Init();
        FileSystem::Init();
        Config::Init();
        eqoverlay = std::make_unique<EQOverlay>();
        if (mouse && keyboard)
        {
            eqoverlay->SetKeyboard(keyboard);
            eqoverlay->SetMouse(mouse);
        }
    });

    return true;
}

bool did_init = false;

extern "C" __declspec(dllexport) bool
SetDevices(IDirectInputDevice8A* k, IDirectInputDevice8A* m)
{
    keyboard = k;
    mouse    = m;
    if (eqoverlay)
    {
        std::cout << "Settings devices for keyboard and mouse" << std::endl;
        eqoverlay->SetKeyboard(k);
        eqoverlay->SetMouse(m);
    }
    else
    {
        std::cout << "Overlay not initialized in SetDevices" << std::endl;
    }
    return true;
}

extern "C" __declspec(dllexport) bool
Initialize(void* callback)
{
    CoreUpdater::SetCallback(callback);
    if (!did_init)
    {
        AttachConsoleToDLL();
        SetupCore();
        did_init = true;
    }

    return true;
}


extern "C" __declspec(dllexport) bool
Teardown()
{
    if (!did_init)
    {
        return true;
    }
    eqoverlay.reset();
    did_init = false;
    Server::Teardown();
    Hooks::Teardown();
    Login::Teardown();
    FileSystem::Teardown();
    Config::Teardown();

    if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
    {
        std::cerr << "Failed to disable hooks." << std::endl;
        return false;
    }

    if (MH_Uninitialize() != MH_OK)
    {
        std::cerr << "Failed to uninitialize MinHook." << std::endl;
        return false;
    }
    std::cout << "Completed core teardown" << std::endl;
    return true;
}

BOOL APIENTRY
DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH: {
            break;
        }
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
