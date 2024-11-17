export module login;

import hooks;
import globals;
import config;
import server;
import eq;
import d3d9_hooks;

import <Windows.h>;
import <MinHook.h>;
import <vector>;
import <string>;
import <unordered_map>;
import <iostream>;
import <iomanip>;
import <tuple>;
import <functional>;
import <thread>;

using login_callback_t    = std::function<bool(int)>;
using login_char_select_t = void(__fastcall*)(uintptr_t This, uintptr_t Reg);
using do_login_t          = int(__fastcall*)(uintptr_t This, uintptr_t Reg, int server_id, uintptr_t user_data, int timeout);
using show_msg_t          = void(__fastcall*)(uintptr_t This, uintptr_t Reg, void* a2, int a3, volatile signed int* a4, void* a5, void* a6);
using process_packet_t    = void(__fastcall*)(uintptr_t This, uintptr_t Reg, unsigned char* data, uintptr_t a2);
using get_tablestring_t   = const char*(__fastcall*) (uintptr_t This, uintptr_t Reg, int entry, void* data);
using load_eqmain         = int(__cdecl*)();

export class Login
{
   public:
    static int LoadEQMain()
    {
        auto res = Original_LoadEQMain();
        CleanupHooks();
        SetupHooks();
        return res;
    }
    static void SetLoginCallback(login_callback_t cb)
    {
        LoginCallback = std::move(cb);
    }

    static void InterceptUnknown(const std::string& msg)
    {
        SkipNextUnknown    = true;
        NextUnknownMessage = msg;
    }

    static const bool InCharSelect()
    {
        return LoggedIntoCharSelect;
    }

    static void LoginToServer(int server_id)
    {
        DoLogin((uintptr_t) eqlib::g_pLoginServerAPI.get(), 0, server_id, 0, 10);
    }

    static void OpenModal(const std::string& msg)
    {
        if (!DialogInstance)
            return;
        int buffer = -1;
        // This is the original "Open this SIDL dialog"
        ((show_msg_t) (eqlib::EQMainBaseAddress + 0xBBE0))(DialogInstance, NULL, NULL, GetOrCreateString(msg), &buffer, nullptr, nullptr);
        // This prevents a check that results in ResetDevice crash, wtf this works.
        uintptr_t MgrInstance = *reinterpret_cast<uintptr_t*>(eqlib::EQMainBaseAddress + 0x150170);
        if (MgrInstance)
        {
            *((BYTE*) MgrInstance + 96) = 0;
        }
    }

    static void SetupHooks()
    {
        D3D9Hooks::Init();
        if (!eqlib::InitializeEQMainOffsets())
        {
            std::cout << "Did not set up hooks ret false" << std::endl;
            return;
        }
        Hooks = {
            {reinterpret_cast<LPVOID>(eqlib::EQMainBaseAddress + 0x13C30), &Login::DoLogin, reinterpret_cast<LPVOID*>(&Original_DoLogin)},
            {reinterpret_cast<LPVOID>(eqlib::EQMainBaseAddress + 0x148F0),
             &Login::ProcessPacket,
             reinterpret_cast<LPVOID*>(&Original_ProcessPacket)},
            {reinterpret_cast<LPVOID>(eqlib::EQMainBaseAddress + 0x26420),
             &Login::GetTableString,
             reinterpret_cast<LPVOID*>(&Original_GetTableString)},
            {reinterpret_cast<LPVOID>(eqlib::EQMainBaseAddress + 0x37DD0),
             &Login::LoginToCharSelect,
             reinterpret_cast<LPVOID*>(&Original_LoginToCharSelect)},
        };

        for (const auto& [target, detour, original] : Hooks)
        {
            if (MH_CreateHook(target, detour, original) != MH_OK || MH_EnableHook(target) != MH_OK)
            {
                std::cout << "Failed enabling hook in login" << std::endl;
            }
        }
        std::cout << "Set up hooks in Login";
    }

    static void CleanupHooks()
    {
        for (const auto& [target, detour, original] : Hooks)
        {
            MH_DisableHook(target);
            MH_RemoveHook(target);
        }
        D3D9Hooks::Teardown();
        Hooks.clear();
        DialogInstance       = 0;
        LoggedIntoCharSelect = false;
    }

    static void Init()
    {
        BaseHooks = {
            {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x1F8E90), &LoadEQMain, reinterpret_cast<LPVOID*>(&Original_LoadEQMain)},
        };
        for (const auto& [target, detour, original] : BaseHooks)
        {
            if (MH_CreateHook(target, detour, original) != MH_OK || MH_EnableHook(target) != MH_OK)
            {
                std::cout << "Failed enabling hook in login" << std::endl;
            }
        }
        SetupHooks();
    }

    static void Teardown()
    {
        CleanupHooks();
        BaseHooks.clear();
        eqlib::CleanupEQMainOffsets();
    }

    static bool DidRetrieveServers()
    {
        return RetrievedServers;
    }

   private:
    inline static do_login_t Original_DoLogin                    = nullptr;
    inline static process_packet_t Original_ProcessPacket        = nullptr;
    inline static get_tablestring_t Original_GetTableString      = nullptr;
    inline static load_eqmain Original_LoadEQMain                = nullptr;
    inline static login_char_select_t Original_LoginToCharSelect = nullptr;
    inline static login_callback_t LoginCallback                 = nullptr;
    inline static uintptr_t LoginInstance                        = 0;
    inline static unsigned long DialogInstance                   = 0;
    inline static bool SkipNextUnknown                           = false;
    inline static bool RetrievedServers                          = false;
    inline static std::string NextUnknownMessage;
    inline static std::unordered_map<int, std::string> StringTableMap = {};
    inline static int StringTableIndex                                = 500'000;
    inline static std::vector<std::tuple<LPVOID, LPVOID, LPVOID*>> Hooks;
    inline static std::vector<std::tuple<LPVOID, LPVOID, LPVOID*>> BaseHooks;
    inline static bool LoggedIntoCharSelect = false;
    static void __fastcall LoginToCharSelect(uintptr_t This, uintptr_t Reg)
    {
        LoggedIntoCharSelect = true;
        return Original_LoginToCharSelect(This, Reg);
    }

    static const char* __fastcall GetTableString(uintptr_t This, uintptr_t Reg, int entry, void* data)
    {
        for (const auto& [key, value] : StringTableMap)
        {
            if (key == entry)
            {
                return value.c_str();
            }
        }
        return Original_GetTableString(This, Reg, entry, data);
    }

    static void __fastcall ProcessPacket(uintptr_t This, uintptr_t Reg, unsigned char* data, uintptr_t a2)
    {
        // This offset is the instance of some class related to SIDL manager that's stored within
        // the loginServerAPI (This) and we need to dereference it to find out where it is.
        if (!DialogInstance)
        {
            DialogInstance = *((unsigned long*) This + 123);
        }
        auto opcode = *((unsigned short*) data + 2);

        switch (opcode)
        {
            case 0x18:  // Server list
                RetrievedServers = true;
                Server::SetContext("");
                break;
            case 0x22:  // PlayEverQuestResponse
            {
                // If we haven't hit login screen yet and are trying to log in, let's validate
                if (!LoggedIntoCharSelect)
                {
                    auto server_id = (int) data[32];
                    if (LoginCallback && LoginCallback(server_id))
                    {
                        OpenModal("");
                        return;
                    }
                }
                if (SkipNextUnknown)
                {
                    SkipNextUnknown = false;
                    OpenModal(NextUnknownMessage);
                    NextUnknownMessage.clear();
                    return;
                }
            }

            default:
                break;
        }
        Original_ProcessPacket(This, Reg, data, a2);
    }

    static uintptr_t __fastcall DoLogin(uintptr_t This, uintptr_t Reg, int server_id, uintptr_t user_data, int timeout)
    {
        if (eqlib::g_pLoginClient && LoginCallback && LoginCallback(server_id))
        {
            return Original_DoLogin(This, Reg, -1, user_data, 0);
        }
        return Original_DoLogin(This, Reg, server_id, user_data, timeout);
    }

    static int GetOrCreateString(const std::string& str)
    {
        for (const auto& [key, value] : StringTableMap)
        {
            if (value == str)
            {
                return key;
            }
        }
        StringTableMap[++StringTableIndex] = str;
        return StringTableIndex;
    }
};
