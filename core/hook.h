// #pragma once
//
// #include <math.h>
// #include <array>
// #include <initializer_list>
// #include <d3d9.h>
// #include <map>
// #include <string>
//
// #include "MinHook.h"
//
// typedef char* (__fastcall* AddZone_t)(int*, char*, unsigned __int32, unsigned __int32, char*,
// char*, unsigned __int32, __int32, __int32, __int32, __int32); typedef int(__fastcall*
// LoadEQStr_t)(DWORD*, char*, const char*, char); typedef char* (__fastcall* GetEQStr_t)(DWORD*,
// char*, DWORD, bool*); typedef int(__fastcall* GetEQItem_t)(int*, char*, unsigned int, int, int);
// typedef unsigned int** (__fastcall* StringTable_AddItem_t)(DWORD*, char*, unsigned int*);
// typedef void(__fastcall* ReceivePacket_t)(DWORD* pthis, char* pChar, void* data, int size, int
// a4, int* portPtr); typedef void(__fastcall* SendPacket_t)(DWORD* pthis, char* pChar, int a2,
// void* a3, size_t size); typedef void(__fastcall* LoginProcessPacket)(void* pthis, char* pChar,
// uint16_t* a2, void* a3);
//
// DWORD baseAddress = (DWORD)GetModuleHandle(NULL);
//
// auto pinstStringTable = (((DWORD)0xDD25C4 - 0x400000) + baseAddress);
// auto AddZone = (((DWORD)0x007DC430 - 0x400000) + baseAddress);
// auto StringTable_GetString = (((DWORD)0x007D0660 - 0x400000) + baseAddress);
// auto StringTable_Load = (((DWORD)0x007D06C0 - 0x400000) + baseAddress);
// auto StringTable_AddItem = (((DWORD)0x007D0540 - 0x400000) + baseAddress);
// auto StringTable_GetItem = (((DWORD)0x007D04F0 - 0x400000) + baseAddress);
//// auto Client_ReceivePacket = (((DWORD)0xB770D0 - 0x6B0000) + baseAddress);
// auto Client_ReceivePacket = (((DWORD)0xB7A460 - 0x6B0000) + baseAddress);
// auto Client_SendPacket = (((DWORD)0xB751F0 - 0x6B0000) + baseAddress);
//
//// Send message
//// char __thiscall sub_B751F0(int this, int a2, _BYTE *a3, size_t Size)
//
// AddZone_t OriginalAddZone = nullptr;
// LoadEQStr_t OriginalStringTable_Load = nullptr;
// GetEQItem_t OriginalStringTable_GetItem = nullptr;
// StringTable_AddItem_t OriginalStringTable_AddItem = nullptr;
// ReceivePacket_t OriginalReceivePacket = nullptr;
// SendPacket_t OriginalSendPacket = nullptr;
// GetEQStr_t GetEQStr = reinterpret_cast<GetEQStr_t>(StringTable_GetString);
//
// struct EQItem {
//    DWORD id;
//    const char* name;
//};
//
// HMODULE hOriginalDll = NULL;
// std::map<std::string, FARPROC> functionCache;
//
// std::wstring ConvertToWide(const std::string& str) {
//    if (str.empty()) {
//        return std::wstring();
//    }
//    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
//    std::wstring wstr(size_needed, 0);
//    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
//    wstr.pop_back();
//    return wstr;
//}
//
// std::string ConvertToNarrow(const std::wstring& wstr) {
//    if (wstr.empty()) {
//        return std::string();
//    }
//    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr,
//    nullptr); std::string str(size_needed, 0); WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1,
//    &str[0], size_needed, nullptr, nullptr); str.pop_back(); return str;
//}
//
// std::string FindOriginalDinput8() {
//    wchar_t systemDir[MAX_PATH];
//    UINT length = GetSystemWow64Directory(systemDir, MAX_PATH);
//
//    if (length == 0 || length >= MAX_PATH) {
//        return "";
//    }
//
//    return ConvertToNarrow(std::wstring(systemDir) + L"\\dinput8.dll");
//}
//
// void LoadOriginalDInput8() {
//    if (!hOriginalDll) {
//        std::string originalDInput8Path = FindOriginalDinput8();
//        hOriginalDll = LoadLibrary(ConvertToWide(originalDInput8Path).c_str());
//        if (!hOriginalDll) {
//            MessageBox(NULL, L"Failed to load original dinput8.dll!", L"Error", MB_OK);
//            ExitProcess(EXIT_FAILURE);
//        }
//    }
//}
//
// FARPROC GetOriginalFunction(const char* funcName) {
//    LoadOriginalDInput8();
//
//    if (auto it = functionCache.find(funcName); it != functionCache.end()) {
//        return it->second;
//    }
//
//    FARPROC proc = GetProcAddress(hOriginalDll, funcName);
//    if (proc) {
//        functionCache[funcName] = proc;
//    }
//    else {
//        MessageBox(NULL, ConvertToWide("Failed to resolve function: " +
//        std::string(funcName)).c_str(), L"Error", MB_OK);
//    }
//
//    return proc;
//}
//
//// Store the last max ID from adding an item so we can increment and create a new id in memory
// DWORD max_id = 0;
//
// unsigned int** __fastcall DetourStringTable_AddItem(DWORD* pThis, char* pPtr, unsigned int* a2) {
//     auto item = reinterpret_cast<EQItem*>(a2);
//     max_id = max_id > item->id ? max_id : item->id;
//     return OriginalStringTable_AddItem(pThis, pPtr, a2);
// }
//
// char* __fastcall DetourAddZone(int* pThis, char* pPtr, unsigned __int32 zoneType, unsigned
// __int32 zoneID, char* zoneShortName, char* zoneLongName, unsigned __int32 eqStrID, __int32
// zoneFlags2, __int32 x, __int32 y, __int32 z) {
//     //auto* itemEntry = new EQItem{ max_id + 1, "Testing String Here" };
//     //DetourStringTable_AddItem((DWORD*)pinstStringTable, nullptr, (unsigned int*)itemEntry);
//     //bool success = false;
//     //auto addedEntry = GetEQStr((DWORD*)pinstStringTable, nullptr, itemEntry->id, &success);
//     return OriginalAddZone(pThis, pPtr, zoneType, zoneID, zoneShortName, zoneLongName, eqStrID,
//     zoneFlags2, x, y, z);
// }
//
// int __fastcall DetourLoadEQStr(DWORD* pThis, char* pPtr, const char* a2, char a3) {
//     return OriginalStringTable_Load(pThis, pPtr, a2, a3);
// }
//
// void __fastcall DetourReceivePacket(DWORD* pthis, char* pChar, void* data, int size, int a4, int*
// portPtr) {
//
//     OriginalReceivePacket(pthis, pChar, data, size, a4, portPtr);
//
//     return;
// }
//
// #pragma pack(1)
//
// struct LoginInfo_Struct {
//     /*000*/	char	login_info[64];
//     /*064*/	char	custom_header[124];
//     /*188*/	uint8_t	zoning;			// 01 if zoning, 00 if not
//     /*189*/	uint8_t	unknown189[275];
//     /*488*/
// };
//
// const uint16_t OP_SendLoginInfo = 0x7a09;
//
//
// void __fastcall DetourSendPacket(DWORD* pthis, char* pChar, int a2, void* data, size_t size) {
//     unsigned char* byteData = reinterpret_cast<unsigned char*>(data);
//     uint16_t opcode = (static_cast<uint16_t>(byteData[1]) << 8) |
//     static_cast<uint16_t>(byteData[0]); switch (opcode) { case OP_SendLoginInfo:
//     {
//         LoginInfo_Struct* li = reinterpret_cast<LoginInfo_Struct*>(byteData + 2);
//         strncpy_s(li->custom_header, sizeof(li->custom_header), "eqcustom", _TRUNCATE);
//         break;
//     }
//     default:
//         break;
//     }
//     OriginalSendPacket(pthis, pChar, a2, data, size);
//     return;
// }
//
// typedef void(__fastcall* SendPacket_t)(DWORD* pthis, char* pChar, int a2, void* a3, size_t size);
//
//
// if (MH_Initialize() != MH_OK) {
//     return FALSE;
// }
//
// std::initializer_list<std::tuple<LPVOID, LPVOID, LPVOID*>> hooks = {
//     { (LPVOID)AddZone, &DetourAddZone, reinterpret_cast<LPVOID*>(&OriginalAddZone) },
//     { (LPVOID)StringTable_Load, &DetourLoadEQStr,
//     reinterpret_cast<LPVOID*>(&OriginalStringTable_Load) }, { (LPVOID)Client_ReceivePacket,
//     &DetourReceivePacket, reinterpret_cast<LPVOID*>(&OriginalReceivePacket) }, {
//     (LPVOID)Client_SendPacket, &DetourSendPacket,
//     reinterpret_cast<LPVOID*>(&OriginalSendPacket)}, { (LPVOID)StringTable_AddItem,
//     &DetourStringTable_AddItem, reinterpret_cast<LPVOID*>(&OriginalStringTable_AddItem) }
// };
//
// for (const auto& [target, detour, original] : hooks) {
//     if (MH_CreateHook(target, detour, original) != MH_OK) {
//         return FALSE;
//     }
//
//     if (MH_EnableHook(target) != MH_OK) {
//         return FALSE;
//     }
// }
//
//
