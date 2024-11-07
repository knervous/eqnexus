export module npcs;

import globals;

import <MinHook.h>;
import <cstring>;
import <vector>;
import <string>;
import <iostream>;
import <unordered_map>;
import <rapidjson/document.h>;

struct NpcConfigEntry {
    int raceId;
    int genderId;
    std::string modelName;
    int raceMask;
    int dbStrId;
};

using add_npc_t = uintptr_t (__fastcall*)(uintptr_t This, uintptr_t Reg, int raceId, int genderId, const char* modelName, int raceMask, int dbStrId);
using npc_manager_ctor_t = uintptr_t (__fastcall*)(uintptr_t This, uintptr_t Reg);

export class NpcConfig {
public:
    static void Init(const rapidjson::Value::Array& npcs) {
        for (const auto& value : npcs) {
            if (!value.IsObject()) {
                continue;
            }
            if (!value["raceId"].IsInt() ||
                !value["genderId"].IsInt() ||
                !value["modelName"].IsString() ||
                !value["raceMask"].IsInt() ||
                !value["dbStrId"].IsInt()) {
                continue;
            }
            AddNpcs.push_back({ 
                value["raceId"].GetInt(),
                value["genderId"].GetInt(),
                value["modelName"].GetString(),
                value["raceMask"].GetInt(),
                value["dbStrId"].GetInt()
             });
        }
        SetupHooks();
    }

    static void Teardown() {
        RemoveHooks();
    }

private:
    inline static std::vector<NpcConfigEntry> AddNpcs = {};
    inline static add_npc_t Original_AddNpc = nullptr;
    inline static npc_manager_ctor_t Original_NpcManagerCtor = nullptr;
    inline static std::vector<std::tuple<LPVOID, LPVOID, LPVOID*>> Hooks;

    static void SetupHooks() {
        Original_AddNpc = (add_npc_t)(eqlib::EQGameBaseAddress + 0x10A440);
        Hooks = {
          {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x10A5F0), &NpcConfig::AddNpcCtor, reinterpret_cast<LPVOID*>(&Original_NpcManagerCtor)},
        };
        for (const auto& [target, detour, original] : Hooks) {
            if (MH_CreateHook(target, detour, original) != MH_OK || MH_EnableHook(target) != MH_OK) {
                std::cout << "Failed enabling hook in config::zone" << std::endl;
            }
        }
    }

    static void RemoveHooks() {
        for (const auto& [target, detour, original] : Hooks) {
            MH_DisableHook(target);
            MH_RemoveHook(target);
        }
        Hooks.clear();
    }

    static uintptr_t __fastcall AddNpcCtor(uintptr_t This, uintptr_t Reg) {
        // First add all the npcs in constructor
        auto mgr = Original_NpcManagerCtor(This, Reg);
        // Then we can call AddNpc
        for (const auto& entry : AddNpcs) {
            Original_AddNpc(mgr, 0, entry.raceId, entry.genderId, entry.modelName.c_str(), entry.raceMask, entry.dbStrId);
        }
        return mgr;
    }

};
