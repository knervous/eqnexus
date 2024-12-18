export module utility;

import <array>;
import <windows.h>;
import <iomanip>;
import <stdexcept>;
import <iostream>;
import <iomanip>;
import <atomic>;
import <memory>;
import <algorithm>;
import <sstream>;
import <cctype>;
import <fstream>;
import <string>;
import <span>;
import <functional>;
import <filesystem>;
import <random>;
import <regex>;
import <format>;
import <xxhash.h>;
import <rapidjson/document.h>;
#include <Psapi.h>
#include <commdlg.h>

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

    std::initializer_list<int>{((void) (pos = formatString.find("{}", pos),
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

const std::string&
GetOptionalString(const rapidjson::Document& obj, const char* key, const std::string& fallback)
{
    if (obj.HasMember(key) && obj[key].IsString())
    {
        return obj[key].GetString();
    }
    return fallback;
}

std::string
GenerateFileHash(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    auto* state = XXH64_createState();
    if (!state)
    {
        std::cerr << "Failed to create xxHash3 state" << std::endl;
        return "";
    }

    XXH64_reset(state, 0);

    if (filePath.find("barter_assets") != std::string::npos)
    {
        auto stop = 213;
        std::cout << filePath << std::endl;
    }

    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0)
    {
        auto count = file.gcount();
        XXH64_update(state, buffer, count);
    }

    XXH64_hash_t hash = XXH64_digest(state);
    XXH64_freeState(state);

    return std::format("{:016X}", hash);
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
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
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
    std::uniform_int_distribution<> dis(0, sizeof(chars) - 2);  // sizeof(chars) - 2 to avoid null terminator

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
toLowerCase(const std::string& input)
{
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
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
ReadIniValue(std::string_view section, std::string_view key, const std::filesystem::path& iniFilePath)
{
    constexpr size_t bufferSize = 256;
    char buffer[bufferSize]{};
    std::span<char> bufferSpan(buffer);

    GetPrivateProfileStringA(
        section.data(), key.data(), "", bufferSpan.data(), static_cast<DWORD>(bufferSpan.size()), iniFilePath.string().c_str());

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

// Function to get the name of the module at a specific address
std::string
GetModuleName(uintptr_t address)
{
    HMODULE hModule           = nullptr;
    char moduleName[MAX_PATH] = {0};

    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR) address, &hModule))
    {
        if (GetModuleFileNameA(hModule, moduleName, MAX_PATH))
        {
            return std::string(moduleName);
        }
    }

    return "<Unknown Module>";
}

// Function to check if an address is hooked and return the target address
bool
IsHooked(uintptr_t address, uintptr_t& hookTarget)
{
    const size_t instructionSize = 5;  // JMP or CALL instructions are 5 bytes long
    BYTE buffer[instructionSize] = {0};

    // Read the first few bytes of the function
    if (!ReadProcessMemory(GetCurrentProcess(), (LPCVOID) address, buffer, sizeof(buffer), nullptr))
    {
        std::cerr << "Failed to read memory at address: 0x" << std::hex << address << std::endl;
        return false;
    }

    // Check for JMP (0xE9) or CALL (0xE8) instructions
    if (buffer[0] == 0xE9 || buffer[0] == 0xE8)
    {
        // Calculate the jump/call target address
        int32_t offset = *reinterpret_cast<int32_t*>(&buffer[1]);
        hookTarget     = address + instructionSize + offset;
        return true;
    }

    return false;
}

class AtomicString
{
   public:
    AtomicString(const std::string& initialValue = "") : atomicStringPtr(std::make_shared<std::string>(initialValue)) {}

    void setString(const std::string& newValue)
    {
        atomicStringPtr.store(std::make_shared<std::string>(newValue));
    }

    std::string getString() const
    {
        auto ptr = atomicStringPtr.load();
        return *ptr;
    }

   private:
    std::atomic<std::shared_ptr<std::string>> atomicStringPtr;
};

bool
ReadFileToString(const std::filesystem::path& filePath, std::string& outContent)
{
    try
    {
        // Check if the file exists and is a regular file
        if (!std::filesystem::exists(filePath))
        {
            std::cerr << "Error: File does not exist: " << filePath << '\n';
            return false;
        }

        if (!std::filesystem::is_regular_file(filePath))
        {
            std::cerr << "Error: Not a regular file: " << filePath << '\n';
            return false;
        }

        // Open the file in binary mode and position the cursor at the end to determine the size
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file)
        {
            std::cerr << "Error: Unable to open file: " << filePath << '\n';
            return false;
        }

        // Determine the file size
        std::streamsize size = file.tellg();
        if (size < 0)
        {
            std::cerr << "Error: Failed to determine the size of the file: " << filePath << '\n';
            return false;
        }

        file.seekg(0, std::ios::beg);  // Reset cursor to the beginning

        // Resize the string to hold the file content
        outContent.resize(static_cast<size_t>(size));

        // Read the file content into the string
        if (!file.read(outContent.data(), size))
        {
            std::cerr << "Error: Failed to read the file: " << filePath << '\n';
            return false;
        }

        return true;
    } catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        return false;
    } catch (const std::exception& e)
    {
        std::cerr << "General error: " << e.what() << '\n';
        return false;
    }
}

}  // namespace util
