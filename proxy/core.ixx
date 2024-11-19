
export module core;

import <dinput.h>;
import "Windows.h";
import <iostream>;
import <cstdio>;
import <thread>;
import <chrono>;
import <filesystem>;

namespace fs = std::filesystem;

typedef void (*UpdateCallback)(const char* folder);
typedef bool (*CoreInitialize_t)(void* update_callback);
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

void
LoadCore();
void
LoadDevices();

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

extern bool __cdecl UpdateCore(const char* temp)
{
    std::cout << "Called update core: " << temp << std::endl;

    if (!fs::exists(temp))
    {
        std::cerr << "Provided folder does not exist!" << std::endl;
        return false;
    }
    // Want to copy this in
    std::string folder(temp);
    // Need to let this function return to core.dll and then do this work on another thread
    // So core.dll can be unloaded
    std::thread t([folder]() {
        // First unload then we copy all the new files in and reload
        UnloadCore();

        fs::path source_eqnexus = fs::path(folder) / "eqnexus";
        if (!fs::exists(source_eqnexus) || !fs::is_directory(source_eqnexus))
        {
            std::cerr << "eqnexus folder not found in the provided folder!" << std::endl;
            return false;
        }

        fs::path target_eqnexus = fs::current_path() / "eqnexus";

        if (!fs::exists(target_eqnexus))
        {
            fs::create_directories(target_eqnexus);
        }

        // Check and copy core.dll first
        fs::path core_source = source_eqnexus / "core.dll";
        fs::path core_target = target_eqnexus / "core.dll";

        if (!fs::exists(core_source))
        {
            std::cerr << "core.dll not found in the provided folder!" << std::endl;
            return false;
        }

        bool copied = false;
        for (int attempt = 1; attempt <= 20; ++attempt)
        {
            UnloadCore();
            
            try
            {
                fs::copy_file(core_source, core_target, fs::copy_options::overwrite_existing);
                std::cout << "Copied core.dll successfully after " << attempt << " attempts: " << core_source << " -> " << core_target
                          << std::endl;
                copied = true;
                break;
            } catch (const fs::filesystem_error& e)
            {
                std::cerr << "Failed to copy core.dll on attempt " << attempt << ": " << e.what() << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        if (!copied)
        {
            MessageBox(NULL,
                       L"Update failed to apply automatically. Please download the latest patch files from the website.",
                       L"Automatic Update",
                       MB_OK);
            try
            {
                fs::remove_all(folder);
                std::cout << "Deleted source folder: " << folder << std::endl;
            } catch (const fs::filesystem_error& e)
            {
                std::cerr << "Error deleting folder: " << e.what() << std::endl;
            }
            return false;
        }

        try
        {
            for (const auto& entry : fs::recursive_directory_iterator(source_eqnexus))
            {
                fs::path relative_path = fs::relative(entry.path(), source_eqnexus);
                fs::path target_path   = target_eqnexus / relative_path;

                if (fs::is_directory(entry))
                {
                    if (!fs::exists(target_path))
                    {
                        fs::create_directories(target_path);
                    }
                }
                else if (fs::is_regular_file(entry))
                {
                    if (fs::exists(target_path))
                    {
                        for (int attempt = 0; attempt < 100; attempt++)
                        {
                            try
                            {
                                fs::copy_file(entry.path(), target_path, fs::copy_options::overwrite_existing);
                                std::cout << "Copied: " << entry.path() << " to " << target_path << std::endl;
                                break;
                            } catch (const fs::filesystem_error& e)
                            {
                                std::cerr << "Filesystem error: " << e.what() << std::endl;
                            }
                            std::this_thread::sleep_for(std::chrono::milliseconds(250));
                        }
                    }
                }
            }
        } catch (const fs::filesystem_error& e)
        {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
            return false;
        }

        try
        {
            fs::remove_all(folder);
            std::cout << "Deleted source folder: " << folder << std::endl;
        } catch (const fs::filesystem_error& e)
        {
            std::cerr << "Error deleting folder: " << e.what() << std::endl;
            return false;
        }

        LoadCore();
        LoadDevices();
    });
    t.detach();

    return true;
}

export void
LoadCore()
{
    auto lib = LoadLibrary(L"eqnexus/core.dll");
    if (lib)
    {
        CoreInitialize_t Initialize = (CoreInitialize_t) GetProcAddress(lib, "Initialize");
        bool success                = Initialize((void*) &UpdateCore);
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
