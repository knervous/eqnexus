import globals;
import overlay;
import d3d9_hooks;

#include <MinHook.h>
#include <iostream>
#include <cstdio>

#include <vector>
#include <dinput.h>

std::unique_ptr<EQOverlay> eqoverlay = nullptr;

// Graphics int __cdecl _sopen_helper(LPCSTR lpFileName, int a2, int a3, int a4, int *a5, int a6)
// Offset .text:10121BFC


////__thiscall sub_46CB80(_DWORD* this, char* FileName)
//int __cdecl _sopen_helper(LPCSTR lpFileName, int a2, int a3, int a4, DWORD* a5, int a6)
// 8CF286
// int __cdecl _stat32(const char *FileName, struct _stat32 *Stat)
// 
// 
// stat
typedef int (__cdecl* fstat32_t)(const char* filename, struct _stat32* Stat);
fstat32_t OriginalfStat = nullptr;

// open
typedef int (__cdecl* fopen_t)(LPCSTR lpFileName, int a2, int a3, int a4, DWORD* a5, int a6);



typedef FILE* (__cdecl* fopen_graphics_t)(const char* filename, const char* mode);
typedef FILE* (__cdecl* fopen_flags_t)(const char* filename, int a2, int flags);
typedef void(__fastcall* fopen2_t)(void* pthis, char* pChar, char* fileName);

fopen_t OriginalFopen = nullptr;
fopen_flags_t OriginalFopenFlags = nullptr;
fopen2_t OriginalFopen2 = nullptr;
fopen_graphics_t OriginalFopenGraphics = nullptr;

bool contains(const std::string& str, const std::string& substring) {
    return str.find(substring) != std::string::npos;
}
int __cdecl HookedFopen(LPCSTR lpFileName, int a2, int a3, int a4, DWORD* a5, int a6) {
    std::string msg(lpFileName);
    if (!contains(msg, "dbg.txt")) {
        std::cout << "Intercepted _fopen for file: " << msg << " with mode: " << a2 << std::endl;
    }
    return OriginalFopen(lpFileName, a2, a3, a4, a5, a6);
}
FILE* __cdecl HookedFopenFlags(const char* filename, int a2, int flags) {
    std::string msg(filename);
    if (!contains(msg, "dbg.txt")) {
        std::cout << "Intercepted _fopen FLAGS for file: " << filename << " with a2: " << a2 << std::endl;
    }
    return OriginalFopenFlags(filename, a2, flags);
}

FILE* __cdecl HookedFopenGraphics(const char* filename, const char* flags) {
    std::string msg(filename);
    if (!contains(msg, "dbg.txt")) {
        std::cout << "Intercepted _fopen GRAPHICS for file: " << filename << std::endl;
    }
    return OriginalFopenGraphics(filename, flags);
}


void __fastcall HookedFopen2(void* pthis, char* pChar, char* fileName) {
    std::cout << "Intercepted _fopen2 for file: " << fileName << std::endl;
    OriginalFopen2(pthis, pChar, fileName);
}


int __cdecl HookedFStat(char* fileName, struct _stat32* Stat) {
    std::cout << "Intercepted FILE STAT for file: " << fileName << std::endl;
    return OriginalfStat(fileName, Stat);
}


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
    // Address of the target function (_fopen in this case)

    const DWORD fStat32Address = (0x8BF450 - 0x3E0000) + (DWORD)GetModuleHandle(0);
    const DWORD fopenAddress = (0x008DCD51 - 0x400000) + (DWORD)GetModuleHandle(0);
    const DWORD fopen2Address = (0x0046CB80 - 0x3E0000) + (DWORD)GetModuleHandle(0);
    const DWORD fopenFlagsAddress = (0x008BAECB - 0x3E0000) + (DWORD)GetModuleHandle(0);
    HMODULE hLibrary = LoadLibrary(L"EQGraphicsDX9.dll");
    const DWORD fopenGraphicsAddress = 0x10F753 + (uintptr_t)hLibrary;
    // void __thiscall sub_46CB80(_DWORD *this, char *FileName)

    // Stat
    if (MH_CreateHook((LPVOID)fStat32Address, (LPVOID)HookedFStat, (LPVOID*)&OriginalfStat) != MH_OK) {
        std::cout << "Failed to create hook for _fopen" << std::endl;
        MH_Uninitialize();
        return FALSE;
    }

    // Enable the hook
    if (MH_EnableHook((LPVOID)fStat32Address) != MH_OK) {
        std::cout << "Failed to enable hook for _fopen" << std::endl;
        MH_RemoveHook((LPVOID)fStat32Address);
        MH_Uninitialize();
        return FALSE;
    }

    // Create the hook for _fopen, pointing it to HookedFopen
    if (MH_CreateHook((LPVOID)fopenAddress, (LPVOID)HookedFopen, (LPVOID*)&OriginalFopen) != MH_OK) {
        std::cout << "Failed to create hook for _fopen" << std::endl;
        MH_Uninitialize();
        return FALSE;
    }

    // Enable the hook
    if (MH_EnableHook((LPVOID)fopenAddress) != MH_OK) {
        std::cout << "Failed to enable hook for _fopen" << std::endl;
        MH_RemoveHook((LPVOID)fopenAddress);
        MH_Uninitialize();
        return FALSE;
    }

    if (MH_CreateHook((LPVOID)fopenGraphicsAddress, (LPVOID)HookedFopenGraphics, (LPVOID*)&OriginalFopenGraphics) != MH_OK) {
        std::cout << "Failed to create hook for _fopen graphics" << std::endl;
        MH_Uninitialize();
        return FALSE;
    }

    // Enable the hook
    if (MH_EnableHook((LPVOID)fopenGraphicsAddress) != MH_OK) {
        std::cout << "Failed to enable hook for _fopen graphics" << std::endl;
        MH_RemoveHook((LPVOID)fopenGraphicsAddress);
        MH_Uninitialize();
        return FALSE;
    }

    // Create the hook for _fopen, pointing it to HookedFopen
    if (MH_CreateHook((LPVOID)fopenFlagsAddress, (LPVOID)HookedFopenFlags, (LPVOID*)&OriginalFopenFlags) != MH_OK) {
        std::cout << "Failed to create hook for _fopen" << std::endl;
        MH_Uninitialize();
        return FALSE;
    }

    // Enable the hook
    if (MH_EnableHook((LPVOID)fopenFlagsAddress) != MH_OK) {
        std::cout << "Failed to enable hook for _fopen" << std::endl;
        MH_RemoveHook((LPVOID)fopenFlagsAddress);
        MH_Uninitialize();
        return FALSE;
    }


    if (MH_CreateHook((LPVOID)fopen2Address, (LPVOID)HookedFopen2, (LPVOID*)&OriginalFopen2) != MH_OK) {
        std::cout << "Failed to create hook for _fopen2" << std::endl;
        MH_Uninitialize();
        return FALSE;
    }

    // Enable the hook
    if (MH_EnableHook((LPVOID)fopen2Address) != MH_OK) {
        std::cout << "Failed to enable hook for _fopen2" << std::endl;
        MH_RemoveHook((LPVOID)fopen2Address);
        MH_Uninitialize();
        return FALSE;
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

