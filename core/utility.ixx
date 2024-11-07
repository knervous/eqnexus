export module utility;

import <windows.h>;
import <iostream>;
import <string>;
import <span>;
import <filesystem>;
import <random>;


export namespace util {
    std::wstring ConvertToWide(const std::string& str) {
        if (str.empty()) {
            return std::wstring();
        }
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
        wstr.pop_back();
        return wstr;
    }

    std::string ConvertToNarrow(const std::wstring& wstr) {
        if (wstr.empty()) {
            return std::string();
        }
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
        str.pop_back();
        return str;
    }


    std::string GenerateShortGUID() {
        static const char chars[] =
            "0123456789"
            "abcdefghijklmnopqrstuvwxyz";

        std::string result;
        result.reserve(8);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(chars) - 2);  // sizeof(chars) - 2 to avoid null terminator

        for (int i = 0; i < 8; ++i) {
            result += chars[dis(gen)];
        }

        return result;
    }

    std::string GetCurrentWorkingDirectory() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
    }

    void OpenExplorerInCustomDirectory(const std::string& subfolder) {
        std::string path = GetCurrentWorkingDirectory() + "\\eqnexus\\" + subfolder;
        ShellExecuteA(NULL, "open", "explorer.exe", path.c_str(), NULL, SW_SHOWNORMAL);
    }

    std::string ReadIniValue(std::string_view section, std::string_view key, const std::filesystem::path& iniFilePath) {
        constexpr size_t bufferSize = 256;
        char buffer[bufferSize]{};
        std::span<char> bufferSpan(buffer);

        GetPrivateProfileStringA(
            section.data(),
            key.data(),
            "",
            bufferSpan.data(),
            static_cast<DWORD>(bufferSpan.size()),
            iniFilePath.string().c_str()
        );

        return std::string(buffer);
    }

}