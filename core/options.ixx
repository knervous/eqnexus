export module options;

import globals;
import utility;

import <Windows.h>;
import <MinHook.h>;
import <cstring>;
import <vector>;
import <cstdint>;
import <set>;
import <string>;
import <tuple>;
import <iostream>;
import <functional>;
import <unordered_map>;
import <rapidjson/document.h>;

class BaseConfigOption
{
   public:
    virtual ~BaseConfigOption() = default;
};

class NoOpConfig : public BaseConfigOption
{
   public:
    NoOpConfig(uintptr_t addr) : addr(addr)
    {
        target = reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + addr);
        if (MH_CreateHook(target, &NoOpConfig::EmptyFunction, &original) != MH_OK)
        {
            std::cout << "Failed to create hook in NoOpConfig::Init" << std::endl;
        }
        else if (MH_EnableHook(target) != MH_OK)
        {
            std::cout << "Failed to enable hook in NoOpConfig::Init" << std::endl;
        }
    }
    ~NoOpConfig() override
    {
        MH_DisableHook(target);
        MH_RemoveHook(target);
    }

   private:
    static uintptr_t __fastcall EmptyFunction(...)
    {
        return 0;
    }
    const uintptr_t addr;
    LPVOID original;
    LPVOID target;
};

using PatchList = std::vector<std::pair<uintptr_t, std::vector<uint8_t>>>;
class PatchConfig : public BaseConfigOption
{
   public:
    PatchConfig(PatchList patches)
    {
        for (const auto& [address, patch] : patches)
        {
            auto restore = PatchA(address, patch.data(), patch.size());
            if (!restore.empty())
            {
                RestorePatches.push_back({address, restore});
            }
        }
    }
    ~PatchConfig() override
    {
        for (const auto& [address, restore] : RestorePatches)
        {
            PatchA(address, restore.data(), restore.size());
        }
    }

   private:
    PatchList RestorePatches = {};
    std::vector<uint8_t> PatchA(uintptr_t addressValue, const void* value, SIZE_T size)
    {
        DWORD oldProtect;
        std::vector<uint8_t> originalBytes(size);
        void* address = (void*) (addressValue + eqlib::EQGameBaseAddress);

        if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            std::cerr << "Failed to change memory protection." << std::endl;
            return {};
        }

        std::memcpy(originalBytes.data(), address, size);
        std::memcpy(address, value, size);

        FlushInstructionCache(GetCurrentProcess(), address, size);

        if (!VirtualProtect(address, size, oldProtect, &oldProtect))
        {
            std::cerr << "Failed to restore memory protection." << std::endl;
        }

        return originalBytes;
    }
};

using HookSet = std::tuple<LPVOID, LPVOID, LPVOID*>;
export class OptionsConfig
{
   public:
    static void Init(const rapidjson::Value::Object& options)
    {
        Options.clear();
        for (const auto& [name, isEnabled] : options)
        {
            if (isEnabled.IsBool() && isEnabled.GetBool() && name.IsString())
            {
                auto handler = optionHandlers.find(name.GetString());
                if (handler != optionHandlers.end())
                {
                    std::cout << "Enabling option: " << name.GetString() << std::endl;
                    handler->second();
                }
                else
                {
                    std::cout << "Unknown option: " << name.GetString() << std::endl;
                }
            }
        }
        SetupHooks();
    }

    static void Teardown()
    {
        Options.clear();
        RemoveHooks();
        for (const auto& fn : CleanupFunctions)
        {
            fn();
        }
        CleanupFunctions.clear();
        State = OptionsState{};
    }

    static std::tuple<bool, std::string> PreventLogin()
    {
        return {State.PreventLogin, State.PreventLoginReason};
    }

   private:
    struct OptionsState {
        bool PreventLogin              = false;
        std::string PreventLoginReason = "";
    };
    inline static OptionsState State                                                    = {};
    inline static std::vector<std::unique_ptr<BaseConfigOption>> Options                = {};
    inline static std::vector<HookSet> Hooks                                            = {};
    inline static std::vector<std::function<void()>> CleanupFunctions                   = {};
    inline static std::unordered_map<std::string, std::function<void()>> optionHandlers = {
        {"disallowMq2",
         []() {
             const std::set<std::string> allowed_dlls = {"entry.dll", "dinput8.dll"};
             auto process_game_events                 = (uintptr_t) (eqlib::EQGameBaseAddress + 0x13A6C0);
             uintptr_t hook_address                   = 0;
             bool is_hooked                           = util::IsHooked(process_game_events, hook_address);
             if (is_hooked)
             {
                 auto name     = util::GetModuleName(hook_address);
                 auto fileName = util::ExtractFilename(name);
                 if (!allowed_dlls.contains(fileName))
                 {
                     State.PreventLogin = true;
                     State.PreventLoginReason =
                         "MacroQuest is not allowed on this server.<br></br>Please stop MacroQuest and restart EverQuest in order to log "
                         "in to "
                         "this server.";
                 }
             }
         }},
        {"disableMapWindow", []() { Options.emplace_back(std::make_unique<NoOpConfig>(0x2CF4A0)); }},
        {"disableLuclinModels",
         []() {
             // This one-off use case to hook this address isn't worthwhile because we can override ini settings elsewhere
             // But I'm keeping this here as a template for anonymous hooks

             /*using load_settings_t = uintptr_t(__cdecl*)(
                 char* lpAppName, char* lpKeyName, char* lpDefault, char* lpReturnedString, size_t nSize, char* lpFileName);
             static load_settings_t OriginalLoadSettings = 0;
             auto load_settings_hook = [](char* name, char* key, char* default_, char* returned, size_t size, char* filename) -> uintptr_t {
                 if (strcmp(name, "Defaults") != 0)
                 {
                     return OriginalLoadSettings(name, key, default_, returned, size, filename);
                 }
                 if (strstr(key, "UseLuclin"))
                 {
                     strncpy(returned, "FALSE", size);
                     returned[size - 1] = '\0';
                     return 5;
                 }
                 return OriginalLoadSettings(name, key, default_, returned, size, filename);
             };
             Hooks.push_back(HookSet{reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x460EF0),
                                     reinterpret_cast<LPVOID>(static_cast<load_settings_t>(load_settings_hook)),
                                     reinterpret_cast<LPVOID*>(&OriginalLoadSettings)});*/
         }},
        {"disableBazaarWindow", []() { Options.emplace_back(std::make_unique<NoOpConfig>(0x236670)); }},
        {"disableHeroic",
         []() {
             Options.emplace_back(std::make_unique<PatchConfig>(PatchList{
                 {0x4410C, {0x90, 0x90, 0xEB}},
                 {0x42B36, {0x90, 0x90, 0xEB}},
                 {0x42BB6, {0x90, 0x90, 0xEB}},
             }));
         }},
        {"disableFoodDrinkSpam",
         []() {
             Options.emplace_back(std::make_unique<PatchConfig>(PatchList{
                 {0x5AE9F, {0x90, 0x90, 0xE9, 0x76, 0x03, 0x00, 0x00, 0x90}},
             }));
         }},
        {"enableMaxHPFix",
         []() {
             Options.emplace_back(std::make_unique<PatchConfig>(PatchList{
                 {0x44158, {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90}},
                 {0x49F64, {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90}},
             }));
         }},
        {"enableAnyClassShield",
         []() {
             Options.emplace_back(std::make_unique<PatchConfig>(PatchList{
                 {0xE8DA8, {0x90, 0x90, 0x90, 0x90, 0x90, 0x90}},
             }));
         }},
        {"enableCombatDamageDoubleAppliedFix",
         []() {
             Options.emplace_back(std::make_unique<PatchConfig>(PatchList{
                 {0x5385D,
                  {
                      0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                      0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                  }},
             }));
         }},
        {"enableOldModelHorseSupport",
         []() {
             Options.emplace_back(std::make_unique<PatchConfig>(PatchList{
                 {0x18DE28, {0x32, 0xC0}},
             }));
         }},
        {"enableUltravision",
         []() {
             Options.emplace_back(std::make_unique<PatchConfig>(PatchList{
                 {0x94455, {0x90, 0x90}},
             }));
         }},
    };

    static void SetupHooks()
    {
        for (const auto& [target, detour, original] : Hooks)
        {
            if (MH_CreateHook(target, detour, original) != MH_OK || MH_EnableHook(target) != MH_OK)
            {
                std::cout << "Failed enabling hook in config::options" << std::endl;
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
};
