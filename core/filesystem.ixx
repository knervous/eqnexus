export module filesystem;

import hooks;
import globals;
import server;

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

export class FileSystem {
public:

	static std::vector<std::tuple<LPVOID, LPVOID, LPVOID*>> hooks;
	typedef char(__stdcall* fstat_t)(char* buffer, const char* filename);
	typedef int(__cdecl* fopen_t)(const char* filename, int a2, int a3, int a4, uintptr_t a5, int a6);
	typedef int(__fastcall* graphics_load_eqg_t)(uintptr_t This, uintptr_t Reg, const char* src, int flag, uintptr_t ptr, const char** strlist);
	typedef int(__fastcall* graphics_load_file_t)(uintptr_t This, uintptr_t Reg, const char* src);
	typedef int(__fastcall* graphics_load_s3d_cache_t)(uintptr_t This, uintptr_t Reg, int **a2, DWORD* a3);
	typedef int(__fastcall* graphics_cache_get_clear_t)(uintptr_t This, uintptr_t Reg);
	typedef int(__fastcall* graphics_cache_write_t)(uintptr_t This, uintptr_t Reg, uintptr_t cache);
	static fopen_t Original_MainOpen;
	static fopen_t Original_GraphicsOpen;
	static fstat_t Original_Stat;
	static graphics_cache_write_t Original_Graphics_CacheWrite;
	static graphics_load_eqg_t Original_Graphics_EqgLoad;
	static graphics_load_file_t Original_Graphics_FileLoad;
	static graphics_load_s3d_cache_t Original_Graphics_S3dLoadCache;
	static uintptr_t GraphicsCache;
	static inline std::vector<HMODULE> libs = {};

	static void Init() {
		Server::RegisterCallback("FileSystem", OnServerContextChanged);
	}
	static void Teardown() {
		Server::RemoveCallback("FileSystem");
	}


	static void ClearCache() {
		if (GraphicsCache) {
			const DWORD fClearCacheAddress = 0xBE060 + eqlib::EQGraphicsBaseAddress;
			auto fn = (FileSystem::graphics_cache_get_clear_t)(fClearCacheAddress);
			auto res = fn(FileSystem::GraphicsCache, 0);
			std::cout << "Result from write: " << res << std::endl;

			fn(FileSystem::GraphicsCache, 0);
		}

	}

	private:
		static void OnServerContextChanged(const std::string& context) {
			if (context.empty()) {
				RemoveHooks();
			}
			else {
				SetupHooks();
			}
		}

		static const std::string GetPrefix() {
			return std::string("eqnexus/") + Server::GetContext() + "/";
		}

		static char __stdcall Stat(char* buffer, const char* filename) {
			if (!Server::GetContext().empty()) {
				auto custom_path = GetPrefix() + filename;

				if (std::filesystem::exists(custom_path)) {
					std::cout << "Stat file custom: " << filename << std::endl;

					return Original_Stat(buffer, custom_path.c_str());
				}
			}

			return Original_Stat(buffer, filename);
		}

		// This cache function does a lookup to see if it's already loaded. We can always short circuit it to force reload from disk.
		// When this hook is on, we just set ret to null. TODO investigate this
		static uintptr_t __fastcall Graphics_S3DLoadCache(uintptr_t This, uintptr_t Reg, int** a2, DWORD* a3) {
			*a2 = nullptr;
			return (uintptr_t)a2;
		}

		static uintptr_t __fastcall Graphics_FileLoad(uintptr_t This, uintptr_t Reg, const char* src) {
			std::cout << "Loading s3d source: " << src << std::endl;
			return Original_Graphics_FileLoad(This, Reg, src);
		}

		static uintptr_t __fastcall Graphics_EQGLoad(uintptr_t This, uintptr_t Reg, const char* src, int flag, uintptr_t ptr, const char** strlist) {
			std::cout << "Loading eqg source: " << src << " with flags" << std::endl;
			return Original_Graphics_EqgLoad(This, Reg, src, flag, ptr, strlist);
		}


		static uintptr_t __fastcall Graphics_CacheWrite(uintptr_t This, uintptr_t Reg, uintptr_t cache) {
			GraphicsCache = This;
			return Original_Graphics_CacheWrite(This, Reg, cache);
		}

		static uintptr_t __cdecl Graphics_FileOpen(const char* filename, int a2, int a3, int a4, uintptr_t a5, int a6) {
			if (!Server::GetContext().empty()) {
				auto custom_path = GetPrefix() + filename;

				if (std::filesystem::exists(custom_path)) {
					std::cout << "Open graphics file custom: " << filename << std::endl;

					return Original_GraphicsOpen(custom_path.c_str(), a2, a3, a4, a5, a6);
				}
			}

			return Original_GraphicsOpen(filename, a2, a3, a4, a5, a6);
		}

		static uintptr_t __cdecl Main_FileOpen(const char* filename, int a2, int a3, int a4, uintptr_t a5, int a6) {
			if (!Server::GetContext().empty()) {
				auto custom_path = GetPrefix() + filename;

				if (std::filesystem::exists(custom_path)) {
					std::cout << "Open graphics file custom: " << filename << std::endl;

					return Original_MainOpen(custom_path.c_str(), a2, a3, a4, a5, a6);
				}
			}

			return Original_MainOpen(filename, a2, a3, a4, a5, a6);
		}
	static void SetupHooks() {
		const auto Graphics_CacheWrite_Addr = eqlib::EQGraphicsBaseAddress + 0xBDF90;
		const auto Graphics_EQG_Load_Addr = eqlib::EQGraphicsBaseAddress + 0x66230;
		const auto Graphics_File_Load_Addr = eqlib::EQGraphicsBaseAddress + 0xBE140;
		const auto Graphics_S3D_Load_Cache_Addr = eqlib::EQGraphicsBaseAddress + 0xDE640;
		const auto Main_FileOpen_Addr = eqlib::EQGameBaseAddress + 0x4DCD51;
		const auto Graphics_FileOpen_Addr = eqlib::EQGraphicsBaseAddress + 0x121BFC;
		const auto Main_Stat_Addr = eqlib::EQGameBaseAddress + 0x89370;

		hooks = {
			std::make_tuple((LPVOID)Graphics_CacheWrite_Addr, &FileSystem::Graphics_CacheWrite, reinterpret_cast<LPVOID*>(&Original_Graphics_CacheWrite)),
			std::make_tuple((LPVOID)Main_Stat_Addr, &FileSystem::Stat, reinterpret_cast<LPVOID*>(&Original_Stat)),
			std::make_tuple((LPVOID)Graphics_EQG_Load_Addr, &FileSystem::Graphics_EQGLoad, reinterpret_cast<LPVOID*>(&Original_Graphics_EqgLoad)),
			std::make_tuple((LPVOID)Graphics_File_Load_Addr, &FileSystem::Graphics_FileLoad, reinterpret_cast<LPVOID*>(&Original_Graphics_FileLoad)),
			std::make_tuple((LPVOID)Graphics_FileOpen_Addr, &FileSystem::Graphics_FileOpen, reinterpret_cast<LPVOID*>(&Original_GraphicsOpen)),
			std::make_tuple((LPVOID)Main_FileOpen_Addr, &FileSystem::Main_FileOpen, reinterpret_cast<LPVOID*>(&Original_MainOpen)),
		};

		for (const auto& [target, detour, original] : hooks) {
			if (MH_CreateHook(target, detour, original) != MH_OK) {
				std::cout << "Failed FS hook create" << std::endl;
				return;
			}

			if (MH_EnableHook(target) != MH_OK) {
				std::cout << "Failed FS hook enable" << std::endl;

				return;
			}
		}

		auto dir = GetPrefix();
		const std::set<std::string> allowed_dlls = {
			"entry.dll",
		};

		for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
			if (entry.path().extension() == ".dll") {
				std::string dll_name = entry.path().filename().string();
				if (allowed_dlls.find(dll_name) != allowed_dlls.end()) {
					HMODULE hModule = LoadLibrary(entry.path().c_str());
					if (hModule) {
						libs.push_back(hModule);
						std::cout << "Loaded DLL: " << dll_name << std::endl;
					}
					else {
						std::cerr << "Failed to load DLL: " << dll_name << std::endl;
					}
				}
			}
		}
	}

	static void RemoveHooks() {
		for (const auto& [target, detour, original] : hooks) {
			MH_DisableHook(reinterpret_cast<void*>(original));
			MH_RemoveHook(reinterpret_cast<void*>(original));
		}
		hooks.clear();

		// Unload all DLLs
		for (HMODULE hModule : libs) {
			if (hModule) {
				FreeLibrary(hModule);
				std::cout << "Unloaded DLL" << std::endl;
			}
		}
		libs.clear();
	}

};

uintptr_t FileSystem::GraphicsCache = 0;
std::vector<std::tuple<LPVOID, LPVOID, LPVOID*>> FileSystem::hooks = {};
FileSystem::graphics_cache_write_t FileSystem::Original_Graphics_CacheWrite = 0;
FileSystem::graphics_load_eqg_t FileSystem::Original_Graphics_EqgLoad = 0;
FileSystem::graphics_load_file_t FileSystem::Original_Graphics_FileLoad = 0;
FileSystem::graphics_load_s3d_cache_t FileSystem::Original_Graphics_S3dLoadCache = 0;
FileSystem::fopen_t FileSystem::Original_MainOpen = 0;
FileSystem::fopen_t FileSystem::Original_GraphicsOpen = 0;
FileSystem::fstat_t FileSystem::Original_Stat = 0;
