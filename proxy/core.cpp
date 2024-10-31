#include "pch.h"
#include "Windows.h"
#include "core.h"
#include <iostream>
#include <cstdio>

typedef bool (*CoreInitialize_t)(void* keyboard, void* mouse);
typedef bool (*CoreTeardown_t)();

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
