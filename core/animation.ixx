export module animations;

import globals;

import <MinHook.h>;
import <cstring>;
import <vector>;
import <string>;
import <iostream>;
import <unordered_map>;
import <rapidjson/document.h>;

struct AnimationConfigEntry {
    std::string originalName;
    std::string replacementName;
};

using add_animation_t = char*(__stdcall*) (char* original, char* replacement, char replaced);

export class AnimationConfig
{
   public:
    static void Init(const rapidjson::Value::Object& animations)
    {
        for (const auto& value : animations)
        {
            if (!value.name.IsString() || !value.value.IsString())
            {
                continue;
            }
            UpdateAnimations.push_back({value.name.GetString(), value.value.GetString()});
        }
        SetupHooks();
    }

    static void Teardown()
    {
        RemoveHooks();
    }

   private:
    inline static std::vector<AnimationConfigEntry> UpdateAnimations = {};
    inline static add_animation_t Original_AddAnimation              = nullptr;
    inline static std::vector<std::tuple<LPVOID, LPVOID, LPVOID*>> Hooks;

    static void SetupHooks()
    {
        Hooks = {
            {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x6A60),
             &AnimationConfig::AddAnimation,
             reinterpret_cast<LPVOID*>(&Original_AddAnimation)},
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
    }

    static char* __stdcall AddAnimation(char* original, char* replacement, char replaced)
    {
        for (const auto& entry : UpdateAnimations)
        {
            if (entry.originalName == original)
            {
                replacement[0] = entry.replacementName[0];
                replacement[1] = entry.replacementName[1];
                if (entry.replacementName.length() == 2)
                {
                    replacement[2] = original[2];
                }
                else
                {
                    replacement[2] = entry.replacementName[2];
                }
                replaced = 0x0;
                return &replaced;
            }
        }
        return Original_AddAnimation(original, replacement, replaced);
    }
};
