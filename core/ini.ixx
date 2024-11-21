export module ini;

import globals;
import utility;

import <MinHook.h>;
import <cstring>;
import <vector>;
import <mutex>;
import <string>;
import <memory>;
import <iostream>;
import <unordered_map>;
import <rapidjson/document.h>;

typedef UINT(__stdcall* GetPrivateProfileIntA_t)(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName);
typedef DWORD(__stdcall* GetPrivateProfileStringA_t)(
    LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName);

using eqclient_getint_t    = int(__cdecl*)(char* section, char* key, int default_value, char* ArgList);
using eqclient_getfloat_t  = double(__stdcall*)(char* section, char* key, float a3);
using eqclient_getstring_t = int(__cdecl*)(char* section, char* key, char* source, char* destination, size_t count, char* ArgList);

export class IniConfig
{
   public:
    static void Init(const rapidjson::Value::Object& overrides)
    {
        std::lock_guard<std::mutex> lockString(stringMapMutex);
        std::lock_guard<std::mutex> lockInt(intMapMutex);
        for (const auto& [filename, options] : overrides)
        {
            if (!filename.IsString() || !options.IsObject())
            {
                continue;
            }
            const auto& iniName = filename.GetString();
            for (const auto& [key, val] : options.GetObj())
            {
                if (val.IsString())
                {
                    if (!iniStringMap.contains(iniName))
                    {
                        iniStringMap[iniName] = {};
                    }
                    const auto& iniValue = std::string(val.GetString());
                    if (!iniValue.empty())
                    {
                        iniStringMap[iniName][key.GetString()] = iniValue;
                    }
                }
                else if (val.IsInt())
                {
                    if (!iniIntMap.contains(iniName))
                    {
                        iniIntMap[iniName] = {};
                    }
                    iniIntMap[iniName][key.GetString()] = val.GetInt();
                }
                else if (val.IsDouble())
                {
                    if (!iniDoubleMap.contains(iniName))
                    {
                        iniDoubleMap[iniName] = {};
                    }
                    iniDoubleMap[iniName][key.GetString()] = val.GetDouble();
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
    inline static std::mutex stringMapMutex;
    inline static std::mutex intMapMutex;
    inline static GetPrivateProfileIntA_t Original_GetPrivateProfileIntA       = nullptr;
    inline static GetPrivateProfileStringA_t Original_GetPrivateProfileStringA = nullptr;
    inline static eqclient_getint_t Original_GetEQClientInt                    = nullptr;
    inline static eqclient_getint_t Original_GetEQClientInt2                   = nullptr;
    inline static eqclient_getfloat_t Original_GetEQClientDouble               = nullptr;
    inline static eqclient_getstring_t Original_GetEQClientString              = nullptr;
    inline static std::vector<std::tuple<LPVOID, LPVOID, LPVOID*>> Hooks;
    inline static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> iniStringMap = {};
    inline static std::unordered_map<std::string, std::unordered_map<std::string, int>> iniIntMap            = {};
    inline static std::unordered_map<std::string, std::unordered_map<std::string, double>> iniDoubleMap      = {};

    static void SetupHooks()
    {
        const auto kernelAddress = GetModuleHandle(L"kernel32.dll");
        Hooks                    = {{
                     reinterpret_cast<LPVOID>(GetProcAddress(kernelAddress, "GetPrivateProfileIntA")),
                     &IniConfig::GetPrivateProfileIntA,
                     reinterpret_cast<LPVOID*>(&Original_GetPrivateProfileIntA),
                 },
                                    {reinterpret_cast<LPVOID>(GetProcAddress(kernelAddress, "GetPrivateProfileStringA")),
                                     &IniConfig::GetPrivateProfileStringA,
                                     reinterpret_cast<LPVOID*>(&Original_GetPrivateProfileStringA)},
                                    {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x460F80),
                                     &IniConfig::GetEQClientInt,
                                     reinterpret_cast<LPVOID*>(&Original_GetEQClientInt)},
                                    {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x460EC0),
                                     &IniConfig::GetEQClientInt2,
                                     reinterpret_cast<LPVOID*>(&Original_GetEQClientInt2)},
                                    {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x143360),
                                     &IniConfig::GetEQClientDouble,
                                     reinterpret_cast<LPVOID*>(&Original_GetEQClientDouble)},
                                    {reinterpret_cast<LPVOID>(eqlib::EQGameBaseAddress + 0x460EF0),
                                     &IniConfig::GetEQClientString,
                                     reinterpret_cast<LPVOID*>(&Original_GetEQClientString)}

        };
        for (const auto& [target, detour, original] : Hooks)
        {
            if (MH_CreateHook(target, detour, original) != MH_OK || MH_EnableHook(target) != MH_OK)
            {
                std::cout << "Failed enabling hook in config::ini" << std::endl;
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

    static int __cdecl GetEQClientInt(char* section, char* key, int default_value, char* ArgList)
    {
        std::cout << "INT: " << section << " | " << key << std::endl;

        if (iniIntMap.contains("eqclient.ini") && iniIntMap["eqclient.ini"].contains(key))
        {
            return iniIntMap["eqclient.ini"][key];
        }
        return Original_GetEQClientInt(section, key, default_value, ArgList);
    }

    static int __cdecl GetEQClientInt2(char* section, char* key, int default_value, char* ArgList)
    {
        if (iniIntMap.contains("eqclient.ini") && iniIntMap["eqclient.ini"].contains(key))
        {
            return iniIntMap["eqclient.ini"][key];
        }
        return Original_GetEQClientInt2(section, key, default_value, ArgList);
    }
    static int __cdecl GetEQClientString(char* section, char* key, char* source, char* destination, size_t count, char* ArgList)
    {
        if (iniStringMap.contains("eqclient.ini") && iniStringMap["eqclient.ini"].contains(key))
        {
            const std::string& str = iniStringMap["eqclient.ini"][key];
            size_t length          = str.length();
            size_t copy_length     = (length < static_cast<size_t>(count - 1)) ? length : static_cast<size_t>(count - 1);

            if (destination)
            {
                std::memcpy(destination, str.c_str(), copy_length);
                destination[copy_length] = '\0';
            }
            return static_cast<DWORD>(copy_length);
        }
        return Original_GetEQClientString(section, key, source, destination, count, ArgList);
    }

    static double __stdcall GetEQClientDouble(char* section, char* key, float default_value)
    {
        if (iniDoubleMap.contains("eqclient.ini") && iniDoubleMap["eqclient.ini"].contains(key))
        {
            return iniDoubleMap["eqclient.ini"][key];
        }
        return Original_GetEQClientDouble(section, key, default_value);
    }

    static UINT __stdcall GetPrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName)
    {
        if (lpFileName != nullptr)
        {
            auto scrubbed_file_name = util::toLowerCase(util::ExtractFilename(lpFileName));

            std::lock_guard<std::mutex> lock(intMapMutex);
            if (iniIntMap.contains(scrubbed_file_name))
            {
                if (iniIntMap[scrubbed_file_name].contains(lpKeyName))
                {
                    return iniIntMap[scrubbed_file_name][lpKeyName];
                }
            }
        }

        return Original_GetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, lpFileName);
    }

    static DWORD __stdcall GetPrivateProfileStringA(
        LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
    {
        if (lpFileName != nullptr)
        {
            std::string scrubbed_file_name = util::toLowerCase(util::ExtractFilename(lpFileName));
            std::lock_guard<std::mutex> lock(stringMapMutex);
            if (iniStringMap.contains(scrubbed_file_name))
            {
                if (iniStringMap[scrubbed_file_name].contains(lpKeyName))
                {
                    const std::string& str = iniStringMap[scrubbed_file_name][lpKeyName];
                    size_t length          = str.length();
                    size_t copy_length     = (length < static_cast<size_t>(nSize - 1)) ? length : static_cast<size_t>(nSize - 1);

                    if (lpReturnedString != nullptr && nSize > 0)
                    {
                        std::memcpy(lpReturnedString, str.c_str(), copy_length);
                        lpReturnedString[copy_length] = '\0';
                    }

                    return static_cast<DWORD>(copy_length);
                }
            }
        }

        return Original_GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
    }
};
