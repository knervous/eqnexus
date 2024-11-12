
export module core;

import <dinput.h>;
import "Windows.h";
import <iostream>;
import <cstdio>;

typedef bool (*CoreInitialize_t)();
typedef bool (*CoreSetDevices_t)(void* keyboard, void* mouse);
typedef bool (*CoreTeardown_t)();

IDirectInputDevice8A* pKeyboardDevice;
IDirectInputDevice8A* pMouseDevice;

export bool core_loaded = false;
export void
SetKeyboardDevice(IDirectInputDevice8A* device)
{
    pKeyboardDevice = device;
};
export IDirectInputDevice8A*
GetKeyboardDevice()
{
    return pKeyboardDevice;
};
export void
SetMouseDevice(IDirectInputDevice8A* device)
{
    pMouseDevice = device;
};
export IDirectInputDevice8A*
GetMouseDevice()
{
    return pMouseDevice;
};

export void
LoadDevices()
{
    auto lib = GetModuleHandle(L"eqnexus/core.dll");
    if (lib)
    {
        CoreSetDevices_t SetDevices = (CoreSetDevices_t) GetProcAddress(lib, "SetDevices");
        bool success                = SetDevices(pKeyboardDevice, pMouseDevice);
        if (success)
        {
            std::cout << "Set Devices success" << std::endl;
        }
        else
        {
            std::cout << "Set Devices failed." << std::endl;
        }
    }
    else
    {
        // MessageBox(NULL, L"Nexus Core not found in eqnexus/core.dll", L"Missing files", MB_OK);
        std::cout << "core.dll not found for LoadDevices";
    }
}

export void
LoadCore()
{
    auto lib = LoadLibrary(L"eqnexus/core.dll");
    if (lib)
    {
        CoreInitialize_t Initialize = (CoreInitialize_t) GetProcAddress(lib, "Initialize");
        bool success                = Initialize();
        if (success)
        {
            std::cout << "Initialization successful!" << std::endl;
            core_loaded = true;
        }
        else
        {
            std::cout << "Initialization failed." << std::endl;
        }
    }
    else
    {
        MessageBox(NULL, L"Nexus Core not found in eqnexus/core.dll", L"Missing files", MB_OK);
    }
}

export void
UnloadCore()
{
    auto lib = GetModuleHandle(L"eqnexus/core.dll");
    if (lib)
    {
        CoreTeardown_t Teardown = (CoreTeardown_t) GetProcAddress(lib, "Teardown");
        bool success            = Teardown();
        if (success)
        {
            std::cout << "Teardown successful!" << std::endl;
            FreeLibrary(lib);
            core_loaded = false;
        }
        else
        {
            std::cout << "Teardown failed." << std::endl;
        }
    }
}
