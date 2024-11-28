export module server_options;

import globals;
import eq;

import <MinHook.h>;
import <cstring>;
import <vector>;
import <string>;
import <iostream>;
import <unordered_map>;
import <rapidjson/document.h>;

using send_packet_t             = void(__fastcall*)(uintptr_t This, uintptr_t Reg, int a2, unsigned char* data, size_t size);
const uint16_t OP_SendLoginInfo = 0x7a09;

export class ServerOptions
{
   public:
    static void Init(const rapidjson::Value::Object& server)
    {
        if (server.HasMember("customFilesKey") && server["customFilesKey"].IsString())
        {
            CustomFilesKey = server["customFilesKey"].GetString();
        }
        SetupHooks();
    }

    static void Teardown()
    {
        CustomFilesKey = "";
        RemoveHooks();
    }

   private:
    inline static std::string CustomFilesKey        = "";
    inline static send_packet_t Original_SendPacket = nullptr;
    inline static std::vector<std::tuple<LPVOID, LPVOID, LPVOID*>> Hooks;

    static void SetupHooks()
    {
        Hooks = {};

        if (!CustomFilesKey.empty())
        {
            Hooks.push_back(std::tuple<LPVOID, LPVOID, LPVOID*>{reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x4C51F0),
                                                                &ServerOptions::SendPacket,
                                                                reinterpret_cast<LPVOID*>(&Original_SendPacket)});
        }
        for (const auto& [target, detour, original] : Hooks)
        {
            if (MH_CreateHook(target, detour, original) != MH_OK || MH_EnableHook(target) != MH_OK)
            {
                std::cout << "Failed enabling hook in config::zone" << std::endl;
            }
        }
    }

    static void RemoveHooks()
    {
        for (const auto& [target, detour, original] : Hooks)
        {
            MH_DisableHook(target);
            MH_RemoveHook(target);
        }
        Hooks.clear();
    }

    static void __fastcall SendPacket(uintptr_t This, uintptr_t Reg, int a2, unsigned char* data, size_t size)
    {
        uint16_t opcode = (static_cast<uint16_t>(data[1]) << 8) | static_cast<uint16_t>(data[0]);
        switch (opcode)
        {
            case OP_SendLoginInfo: {
                LoginInfo_Struct* li = reinterpret_cast<LoginInfo_Struct*>(data + 2);
                strncpy_s(li->custom_header, sizeof(li->custom_header), CustomFilesKey.c_str(), _TRUNCATE);
                break;
            }
            default:
                break;
        }
        return Original_SendPacket(This, Reg, a2, data, size);
    }
};
