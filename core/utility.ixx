export module utility;

import <array>;
import <windows.h>;
import <iomanip>;
import <stdexcept>;
import <iostream>;
import <sstream>;
import <fstream>;
import <string>;
import <span>;
import <functional>;
import <filesystem>;
import <random>;
import <regex>;
import <format>;
import <rapidjson/document.h>;
#include <commdlg.h>

#include "wincrypt.h"

static const std::array<uint32_t, 256> crc32_table = [] {
    std::array<uint32_t, 256> table = {};
    uint32_t polynomial             = 0xEDB88320;
    for (uint32_t i = 0; i < 256; ++i)
    {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; ++j)
        {
            crc = (crc >> 1) ^ (crc & 1 ? polynomial : 0);
        }
        table[i] = crc;
    }
    return table;
}();

namespace fs = std::filesystem;

export namespace util
{

template <typename T>
void
format_argument(std::ostringstream& oss, const T& arg)
{
    oss << arg;
}

template <typename... Args>
std::string
Interpolate(const std::string& formatString, Args&&... args)
{
    std::ostringstream oss;
    std::size_t pos = 0, placeholderIdx = 0;

    std::initializer_list<int>{
        ((void) (pos = formatString.find("{}", pos),
                 oss << formatString.substr(placeholderIdx, pos - placeholderIdx),
                 oss << args,
                 pos += 2,  // Move past "{}"
                 placeholderIdx = pos),
         0)...};

    oss << formatString.substr(placeholderIdx);
    return oss.str();
}

template <typename T>
T
GetWithFallback(const rapidjson::Value& obj, const char* key, T fallback)
{
    if (obj.HasMember(key))
    {
        const rapidjson::Value& value = obj[key];

        if constexpr (std::is_same_v<T, int> && value.IsInt())
        {
            return value.GetInt();
        }
        else if constexpr (std::is_same_v<T, double> && value.IsDouble())
        {
            return value.GetDouble();
        }
        else if constexpr (std::is_same_v<T, std::string_view> && value.IsString())
        {
            return std::string_view(value.GetString(), value.GetStringLength());
        }
    }
    return fallback;
}

const std::string& GetOptionalString(const rapidjson::Document& obj, const char* key, const std::string& fallback)
{
    if (obj.HasMember(key) && obj[key].IsString())
    {
        return obj[key].GetString();
    }
    return fallback;
}

std::string
GetMD5Hash(const std::string& filename)
{
    // Open the file
    std::ifstream file(filename, std::ifstream::binary);
    if (!file)
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    // Initialize the crypto provider
    HCRYPTPROV hProv = 0;
    if (!CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        throw std::runtime_error("CryptAcquireContext failed");
    }

    // Initialize the MD5 hash object
    HCRYPTHASH hHash = 0;
    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    {
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("CryptCreateHash failed");
    }

    // Read and hash file contents
    char buffer[8192];
    while (file.good())
    {
        file.read(buffer, sizeof(buffer));
        if (!CryptHashData(hHash, reinterpret_cast<BYTE*>(buffer), file.gcount(), 0))
        {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            throw std::runtime_error("CryptHashData failed");
        }
    }
    file.close();

    // Retrieve the hash value
    BYTE hash[16];
    DWORD hashSize = sizeof(hash);
    if (!CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashSize, 0))
    {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        throw std::runtime_error("CryptGetHashParam failed");
    }

    // Clean up
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    // Convert the hash to a hex string
    std::stringstream ss;
    for (DWORD i = 0; i < hashSize; ++i)
    {
        ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
           << static_cast<int>(hash[i]);
    }
    return ss.str();
}

void
ReplaceAll(std::string& str, const std::string& from, const std::string& to)
{
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos)
    {
        str.replace(startPos, from.length(), to);
        startPos += to.length();  // Move past the last replaced position
    }
}
std::wstring
ConvertToWide(const std::string& str)
{
    if (str.empty())
    {
        return std::wstring();
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    wstr.pop_back();
    return wstr;
}

std::string
ConvertToNarrow(const std::wstring& wstr)
{
    if (wstr.empty())
    {
        return std::string();
    }
    int size_needed =
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    str.pop_back();
    return str;
}

std::string
GenerateShortGUID()
{
    static const char chars[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";

    std::string result;
    result.reserve(8);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(
        0, sizeof(chars) - 2);  // sizeof(chars) - 2 to avoid null terminator

    for (int i = 0; i < 8; ++i)
    {
        result += chars[dis(gen)];
    }

    return result;
}

std::string
ToStringWithPrecision(double value, int precision = 2)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

std::string
GetCurrentWorkingDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

void
OpenExplorerInCustomDirectory(const std::string& subfolder)
{
    std::string path = GetCurrentWorkingDirectory() + "\\eqnexus\\" + subfolder;
    ShellExecuteA(NULL, "open", "explorer.exe", path.c_str(), NULL, SW_SHOWNORMAL);
}

std::string
ReadIniValue(std::string_view section,
             std::string_view key,
             const std::filesystem::path& iniFilePath)
{
    constexpr size_t bufferSize = 256;
    char buffer[bufferSize]{};
    std::span<char> bufferSpan(buffer);

    GetPrivateProfileStringA(section.data(),
                             key.data(),
                             "",
                             bufferSpan.data(),
                             static_cast<DWORD>(bufferSpan.size()),
                             iniFilePath.string().c_str());

    return std::string(buffer);
}
std::string
OpenFileDialog()
{
    OPENFILENAME ofn;
    wchar_t szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = nullptr;
    ofn.lpstrFile       = szFile;
    ofn.nMaxFile        = sizeof(szFile);
    ofn.lpstrFilter     = L"Custom Server Bundle (*.zip)\0*.zip\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = nullptr;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        return ConvertToNarrow(ofn.lpstrFile);
    }
    return std::string();
}

std::string
ExtractFilename(const std::string& filePath)
{
    std::regex fileRegex(R"(([^\\/]+)$)");
    std::smatch match;

    if (std::regex_search(filePath, match, fileRegex))
    {
        return match[1];
    }
    else
    {
        return "";
    }
}

uint32_t
UpdateCRC32(uint32_t crc, const char* data, std::size_t length)
{
    crc = ~crc;
    for (std::size_t i = 0; i < length; ++i)
    {
        crc = (crc >> 8) ^ crc32_table[(crc ^ static_cast<uint8_t>(data[i])) & 0xFF];
    }
    return ~crc;
}

std::string
CRC32ToHex(uint32_t crc)
{
    std::ostringstream oss;
    oss << std::hex << std::setw(8) << std::setfill('0') << crc;
    return oss.str();
}

uint32_t
ComputeFileCRC(const fs::path& filePath)
{
    std::ifstream fileStream(filePath, std::ios::binary);
    if (!fileStream)
    {
        std::cerr << "Could not open file: " << filePath << std::endl;
        return 0;
    }

    uint32_t crc = 0xFFFFFFFF;
    char buffer[4096];
    while (fileStream.read(buffer, sizeof(buffer)) || fileStream.gcount() > 0)
    {
        crc = UpdateCRC32(crc, buffer, fileStream.gcount());
    }
    return crc ^ 0xFFFFFFFF;  // Finalize the CRC
}

std::string
ComputeDirectoryCRC(const fs::path& directoryPath, std::function<void(std::string)> cb = 0)
{
    uint32_t cumulativeCRC = 0xFFFFFFFF;

    for (const auto& entry : fs::directory_iterator(directoryPath))
    {
        if (entry.is_regular_file())
        {
            const std::string fileName = entry.path().filename().string();

            // Skip  files
            if (fileName == "version.txt" || fileName == "hash.txt")
            {
                continue;
            }

            // Compute CRC32 for each file and combine it
            uint32_t fileCRC = ComputeFileCRC(entry.path());
            if (cb)
            {
                cb(entry.path().string());
            }
            cumulativeCRC ^= fileCRC;  // XOR to combine CRCs
        }
    }

    // Convert cumulative CRC to a hexadecimal string
    return CRC32ToHex(cumulativeCRC ^ 0xFFFFFFFF);  // Finalize cumulative CRC
}

}  // namespace util
