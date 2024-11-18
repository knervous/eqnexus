export module filesystem;

import hooks;
import globals;
import server;
import utility;

import <Windows.h>;
import <MinHook.h>;
import <vector>;
import <initializer_list>;
import <string>;
import <map>;
import <iostream>;
import <filesystem>;
import <tuple>;
import <set>;
import <thread>;

using HookEntry           = std::tuple<LPVOID, LPVOID, LPVOID*>;
using stat32_t            = int(__cdecl*)(const char* filename, struct _stat32* stat);
using fstat_t             = char(__stdcall*)(char* buffer, const char* filename);
using fopen_t             = int(__cdecl*)(const char* filename, int a2, int a3, int a4, uintptr_t a5, int a6);
using checksum_t          = int(__cdecl*)(int a1);
using graphics_load_eqg_t = int(__fastcall*)(uintptr_t This, uintptr_t Reg, const char* src, int flag, uintptr_t ptr, const char** strlist);
using graphics_load_file_t       = int(__fastcall*)(uintptr_t This, uintptr_t Reg, const char* src);
using graphics_load_s3d_cache_t  = int(__fastcall*)(uintptr_t This, uintptr_t Reg, int** a2, DWORD* a3);
using graphics_cache_get_clear_t = int(__fastcall*)(uintptr_t This, uintptr_t Reg);
using graphics_cache_write_t     = int(__fastcall*)(uintptr_t This, uintptr_t Reg, uintptr_t cache);

export class FileSystem
{
   public:
    static void Init()
    {
        Server::RegisterCallback("FileSystem", OnServerContextChanged);
    }

    static void Teardown()
    {
        Server::RemoveCallback("FileSystem");
    }

    static void ClearCache()
    {
        if (GraphicsCache)
        {
            const DWORD fClearCacheAddress = 0xBE060 + eqlib::EQGraphicsBaseAddress;
            auto fn                        = (graphics_cache_get_clear_t) (fClearCacheAddress);
            auto res                       = fn(GraphicsCache, 0);
            std::cout << "Result from write: " << res << std::endl;
            fn(GraphicsCache, 0);
        }
    }

   private:
    inline static std::vector<HookEntry> hooks;
    inline static fopen_t Original_MainOpen                                = nullptr;
    inline static fopen_t Original_GraphicsOpen                            = nullptr;
    inline static fstat_t Original_Stat                                    = nullptr;
    inline static stat32_t Original_Stat32                                 = nullptr;
    inline static checksum_t Original_SkillCaps                            = nullptr;
    inline static checksum_t Original_BaseData                             = nullptr;
    inline static graphics_cache_write_t Original_Graphics_CacheWrite      = nullptr;
    inline static graphics_load_eqg_t Original_Graphics_EqgLoad            = nullptr;
    inline static graphics_load_file_t Original_Graphics_FileLoad          = nullptr;
    inline static graphics_load_s3d_cache_t Original_Graphics_S3dLoadCache = nullptr;
    inline static uintptr_t GraphicsCache                                  = 0;
    inline static std::vector<HMODULE> libs                                = {};
    static void OnServerContextChanged(const std::string& context)
    {
        if (context.empty())
        {
            RemoveHooks();
        }
        else
        {
            SetupHooks();
        }
    }

    static const std::string GetPrefix()
    {
        return std::string("eqnexus/") + Server::GetContext() + "/";
    }

    static char __stdcall Stat(char* buffer, const char* filename)
    {
        if (!Server::GetContext().empty())
        {
            auto custom_path = GetPrefix() + filename;
            if (std::filesystem::exists(custom_path))
            {
                std::cout << "Stat file custom: " << filename << std::endl;
                return Original_Stat(buffer, custom_path.c_str());
            }
        }
        return Original_Stat(buffer, filename);
    }

    static int __cdecl Checksum(int a1)
    {
        return 1;
    }

    static char __cdecl Stat32(const char* filename, struct _stat32* stat)
    {
        if (!Server::GetContext().empty())
        {
            auto custom_path = GetPrefix() + filename;

            std::filesystem::path absolute_custom_path = std::filesystem::absolute(custom_path);

            std::string absolute_custom_path_str = absolute_custom_path.string();
            std::replace(absolute_custom_path_str.begin(), absolute_custom_path_str.end(), '\\', '/');
            // This needs to be absolute due to custom stat32 impl
            if (std::filesystem::exists(absolute_custom_path_str))
            {
                std::cout << "Stat32 file custom: " << absolute_custom_path_str << std::endl;
                auto ret = Original_Stat32(absolute_custom_path_str.c_str(), stat);
                return ret;
            }
        }
        return Original_Stat32(filename, stat);
    }

    static uintptr_t __fastcall Graphics_S3DLoadCache(uintptr_t This, uintptr_t Reg, int** a2, DWORD* a3)
    {
        *a2 = nullptr;
        return reinterpret_cast<uintptr_t>(a2);
    }

    static uintptr_t __fastcall Graphics_FileLoad(uintptr_t This, uintptr_t Reg, const char* src)
    {
        std::cout << "Loading s3d source: " << src << std::endl;
        return Original_Graphics_FileLoad(This, Reg, src);
    }

    static uintptr_t __fastcall Graphics_EQGLoad(
        uintptr_t This, uintptr_t Reg, const char* src, int flag, uintptr_t ptr, const char** strlist)
    {
        std::cout << "Loading eqg source: " << src << " with flags" << std::endl;
        if (!Server::GetContext().empty())
        {
            std::string eqg_source(src);
            util::ReplaceAll(eqg_source, "EQG", "s3d");
            auto custom_path = GetPrefix() + eqg_source;
            if (std::filesystem::exists(custom_path))
            {
                std::cout << "Open eqg source custom: " << custom_path << std::endl;
                return Original_Graphics_EqgLoad(This, Reg, custom_path.c_str(), flag, ptr, strlist);
            }
        }
        return Original_Graphics_EqgLoad(This, Reg, src, flag, ptr, strlist);
    }

    static uintptr_t __fastcall Graphics_CacheWrite(uintptr_t This, uintptr_t Reg, uintptr_t cache)
    {
        GraphicsCache = This;
        return Original_Graphics_CacheWrite(This, Reg, cache);
    }

    static uintptr_t __cdecl Graphics_FileOpen(const char* filename, int a2, int a3, int a4, uintptr_t a5, int a6)
    {
        if (!Server::GetContext().empty())
        {
            auto custom_path = GetPrefix() + filename;
            if (std::filesystem::exists(custom_path))
            {
                std::cout << "Open graphics file custom: " << filename << std::endl;
                return Original_GraphicsOpen(custom_path.c_str(), a2, a3, a4, a5, a6);
            }
        }
        return Original_GraphicsOpen(filename, a2, a3, a4, a5, a6);
    }

    static uintptr_t __cdecl Main_FileOpen(const char* filename, int a2, int a3, int a4, uintptr_t a5, int a6)
    {
        if (!Server::GetContext().empty())
        {
            auto custom_path = GetPrefix() + filename;
            if (std::filesystem::exists(custom_path))
            {
                std::cout << "EQGame file custom: " << filename << std::endl;
                return Original_MainOpen(custom_path.c_str(), a2, a3, a4, a5, a6);
            }
        }
        return Original_MainOpen(filename, a2, a3, a4, a5, a6);
    }

    static void SetupHooks()
    {
        const auto Graphics_CacheWrite_Addr     = eqlib::EQGraphicsBaseAddress + 0xBDF90;
        const auto Graphics_EQG_Load_Addr       = eqlib::EQGraphicsBaseAddress + 0x66230;
        const auto Graphics_File_Load_Addr      = eqlib::EQGraphicsBaseAddress + 0xBE140;
        const auto Graphics_S3D_Load_Cache_Addr = eqlib::EQGraphicsBaseAddress + 0xDE640;
        const auto Graphics_FileOpen_Addr       = eqlib::EQGraphicsBaseAddress + 0x121BFC;
        const auto Main_FileOpen_Addr           = eqlib::EQGameBaseAddress + 0x4DCD51;
        const auto Main_Stat_Addr               = eqlib::EQGameBaseAddress + 0x89370;
        const auto Main_Stat32_Addr             = eqlib::EQGameBaseAddress + 0x4DF450;
        const auto Main_SkillCaps_Addr          = eqlib::EQGameBaseAddress + 0xEE8A0;
        const auto Main_BaseData_Addr           = eqlib::EQGameBaseAddress + 0xEEAA0;

        hooks = {
            {(LPVOID) Graphics_CacheWrite_Addr, &FileSystem::Graphics_CacheWrite, reinterpret_cast<LPVOID*>(&Original_Graphics_CacheWrite)},
            {(LPVOID) Main_Stat_Addr, &FileSystem::Stat, reinterpret_cast<LPVOID*>(&Original_Stat)},
            {(LPVOID) Main_Stat32_Addr, &FileSystem::Stat32, reinterpret_cast<LPVOID*>(&Original_Stat32)},
            {(LPVOID) Graphics_EQG_Load_Addr, &FileSystem::Graphics_EQGLoad, reinterpret_cast<LPVOID*>(&Original_Graphics_EqgLoad)},
            {(LPVOID) Graphics_File_Load_Addr, &FileSystem::Graphics_FileLoad, reinterpret_cast<LPVOID*>(&Original_Graphics_FileLoad)},
            {(LPVOID) Graphics_FileOpen_Addr, &FileSystem::Graphics_FileOpen, reinterpret_cast<LPVOID*>(&Original_GraphicsOpen)},
            {(LPVOID) Main_FileOpen_Addr, &FileSystem::Main_FileOpen, reinterpret_cast<LPVOID*>(&Original_MainOpen)},
            {(LPVOID) Main_SkillCaps_Addr, &FileSystem::Checksum, reinterpret_cast<LPVOID*>(&Original_SkillCaps)},
            {(LPVOID) Main_BaseData_Addr, &FileSystem::Checksum, reinterpret_cast<LPVOID*>(&Original_BaseData)},

        };

        for (const auto& [target, detour, original] : hooks)
        {
            if (MH_CreateHook(target, detour, original) != MH_OK)
            {
                std::cout << "Failed FS hook create" << std::endl;
                return;
            }

            if (MH_EnableHook(target) != MH_OK)
            {
                std::cout << "Failed FS hook enable" << std::endl;
                return;
            }
        }

        const std::set<std::string> allowed_dlls = {"entry.dll", "dinput8.dll"};
        for (const auto& entry : std::filesystem::recursive_directory_iterator(GetPrefix()))
        {         
            if (entry.path().extension() == ".dll")
            {
                std::string dll_name = entry.path().filename().string();
                if (allowed_dlls.contains(dll_name))
                {
                    if (HMODULE hModule = LoadLibrary(entry.path().c_str()))
                    {
                        libs.push_back(hModule);
                        std::cout << "Loaded DLL: " << dll_name << std::endl;
                        break;
                    }
                    else
                    {
                        std::cerr << "Failed to load DLL: " << dll_name << std::endl;
                    }
                }
            }
        }
    }

    static void RemoveHooks()
    {
        for (const auto& [target, detour, original] : hooks)
        {
            MH_DisableHook(reinterpret_cast<void*>(original));
            MH_RemoveHook(reinterpret_cast<void*>(original));
        }
        hooks.clear();

        for (HMODULE hModule : libs)
        {
            if (hModule)
            {
                FreeLibrary(hModule);
                std::cout << "Unloaded DLL" << std::endl;
            }
        }
        libs.clear();
    }
};
