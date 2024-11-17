export module zones;

import globals;
import eq;

import <MinHook.h>;
import <vector>;
import <string>;
import <iostream>;
import <unordered_map>;
import <rapidjson/document.h>;

void
AssignIfPresent(const rapidjson::Value::ConstObject& obj, const char* fieldName, int& target)
{
    if (obj.HasMember(fieldName) && obj[fieldName].IsInt())
    {
        target = obj[fieldName].GetInt();
    }
}

void
AssignIfPresent(const rapidjson::Value::ConstObject& obj, const char* fieldName, std::string& target)
{
    if (obj.HasMember(fieldName) && obj[fieldName].IsString())
    {
        target = obj[fieldName].GetString();
    }
}

struct ZoneConfigEntry {
    int zoneType;
    int zoneId;
    std::string shortName;
    std::string longName;
    int strId;
    int flags;
    int x;
    int y;
    int z;
};

ZoneConfigEntry
CreateZoneConfig(const rapidjson::Value::ConstObject& value, const ZoneConfigEntry& defaults)
{
    ZoneConfigEntry z = defaults;
    AssignIfPresent(value, "zoneType", z.zoneType);
    AssignIfPresent(value, "zoneId", z.zoneId);
    AssignIfPresent(value, "shortName", z.shortName);
    AssignIfPresent(value, "longName", z.longName);
    AssignIfPresent(value, "strId", z.strId);
    AssignIfPresent(value, "flags", z.flags);
    AssignIfPresent(value, "x", z.x);
    AssignIfPresent(value, "y", z.y);
    AssignIfPresent(value, "z", z.z);
    return z;
}

using zone_ctor_t       = ZoneManager*(__fastcall*) (ZoneManager* This, uintptr_t Reg);
using zone_entry_add_t  = uintptr_t(__fastcall*)(uintptr_t This,
                                                uintptr_t Reg,
                                                int zone_type,
                                                int zone_id,
                                                const char* short_name,
                                                const char* long_name,
                                                int str_id,
                                                int zone_flags,
                                                int x,
                                                int y,
                                                int z);
using get_tablestring_t = const char*(__fastcall*) (uintptr_t This, uintptr_t Reg, int entry, void* data);

export class ZoneConfig
{
   public:
    static void Init(const rapidjson::Value::Object& zone)
    {
        if (zone.HasMember("add") && zone["add"].IsArray())
        {
            for (const auto& value : zone["add"].GetArray())
            {
                if (value.IsObject())
                {
                    AddZones.push_back(CreateZoneConfig(value.GetObj(), {0, -1, "", "", 1, 0, 0, 0, 0}));
                }
            }
        }

        if (zone.HasMember("update") && zone["update"].IsObject())
        {
            for (const auto& member : zone["update"].GetObj())
            {
                const std::string key = member.name.GetString();
                const auto& value     = member.value;
                if (value.IsObject())
                {
                    UpdateZones.push_back(CreateZoneConfig(value.GetObj(), {-1, -1, key, "", -1, -1, -1, -1, -1}));
                }
            }
        }

        SetupHooks();
    }

    static void Teardown()
    {
        RemoveHooks();
    }

   private:
    inline static std::vector<ZoneConfigEntry> AddZones     = {};
    inline static std::vector<ZoneConfigEntry> UpdateZones  = {};
    inline static ZoneManager* ZoneManagerInstance          = nullptr;
    inline static zone_ctor_t Original_ZoneCtor             = nullptr;
    inline static zone_entry_add_t Original_ZoneEntryCtor   = nullptr;
    inline static zone_entry_add_t Original_AddZoneEntry    = nullptr;
    inline static get_tablestring_t Original_GetTableString = nullptr;
    inline static std::vector<std::tuple<LPVOID, LPVOID, LPVOID*>> Hooks;
    inline static std::unordered_map<int, std::string> StringTableMap = {};
    inline static int StringTableIndex                                = 500'000;

    static void SetupHooks()
    {
        Original_AddZoneEntry = (zone_entry_add_t) (eqlib::EQGameBaseAddress + 0x3DC430);
        Hooks                 = {
            {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x3DCA10),
                             &ZoneConfig::ZoneCtor,
                             reinterpret_cast<LPVOID*>(&Original_ZoneCtor)},
            {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x3DC290),
                             &ZoneConfig::ZoneEntryCtor,
                             reinterpret_cast<LPVOID*>(&Original_ZoneEntryCtor)},
            {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x3D0660),
                             &ZoneConfig::GetTableString,
                             reinterpret_cast<LPVOID*>(&Original_GetTableString)},
        };
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
        StringTableMap.clear();
    }

    static ZoneManager* __fastcall ZoneCtor(ZoneManager* This, uintptr_t Reg)
    {
        auto result         = Original_ZoneCtor(This, Reg);
        ZoneManagerInstance = result;

        // We've run through the constructor for ZoneManager which means we've added all the zones
        // internal to eqgame.exe and updated any along the way with our updated list. Now we can
        // add our own.
        for (const auto& entry : AddZones)
        {
            if (entry.zoneId == -1)
            {
                continue;
            }
            Original_AddZoneEntry((uintptr_t) ZoneManagerInstance,
                                  0,
                                  entry.zoneType,
                                  entry.zoneId,
                                  entry.shortName.c_str(),
                                  entry.longName.c_str(),
                                  GetOrCreateString(entry.longName),
                                  entry.flags,
                                  entry.x,
                                  entry.y,
                                  entry.z);
        }
        return result;
    }

    static uintptr_t __fastcall ZoneEntryCtor(uintptr_t This,
                                              uintptr_t Reg,
                                              int zone_type,
                                              int zone_id,
                                              const char* short_name,
                                              const char* long_name,
                                              int str_id,
                                              int zone_flags,
                                              int x,
                                              int y,
                                              int z)
    {
        for (const auto& entry : UpdateZones)
        {
            if (short_name == entry.shortName)
            {
                // -1 is sentinel value for default values, i.e. take what we already have provided,
                // otherwise take the provided value
                return Original_ZoneEntryCtor(This,
                                              Reg,
                                              entry.zoneType != -1 ? entry.zoneType : zone_type,
                                              entry.zoneId != -1 ? entry.zoneId : zone_id,
                                              short_name,
                                              !entry.longName.empty() ? entry.longName.c_str() : long_name,
                                              !entry.longName.empty() ? GetOrCreateString(entry.longName) : str_id,
                                              entry.flags != -1 ? entry.flags : zone_flags,
                                              entry.x != -1 ? entry.x : x,
                                              entry.y != -1 ? entry.y : y,
                                              entry.z != -1 ? entry.z : z);
            }
        }
        return Original_ZoneEntryCtor(This, Reg, zone_type, zone_id, short_name, long_name, str_id, zone_flags, x, y, z);
    }

    static const char* __fastcall GetTableString(uintptr_t This, uintptr_t Reg, int entry, void* data)
    {
        if (StringTableMap.contains(entry))
        {
            return StringTableMap[entry].c_str();
        }
        return Original_GetTableString(This, Reg, entry, data);
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
