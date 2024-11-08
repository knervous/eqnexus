export module http;

import <Windows.h>;
import <numeric>;
import <string>;
import <fstream>;
import <iostream>;
import <functional>;
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

constexpr auto* user_agent = L"EQNexus";

std::wstring StringToWString(const std::string& str) {
    std::wstring wstr(str.begin(), str.end());
    return wstr;
}

// Export functions in the module
export namespace http {
    // Fetch JSON data as a string
    std::string DownloadJson(const std::string& url) {
        HINTERNET hInternet = InternetOpen(user_agent, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
        if (!hInternet) return "";

        HINTERNET hConnect = InternetOpenUrl(
            hInternet,
            StringToWString(url).c_str(),
            NULL,
            0,
            INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID,
            0
        );
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return "";
        }

        char buffer[1024];
        DWORD bytesRead;
        std::string result;

        while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead != 0) {
            buffer[bytesRead] = 0;
            result += buffer;
        }

        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return result;
    }

    bool DownloadBinary(const std::string& url, const std::string& outputFilePath, std::function<void(double)> progress_callback = nullptr) {
        HINTERNET hInternet = InternetOpen(user_agent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) return false;

        HINTERNET hConnect = InternetOpenUrl(
            hInternet,
            StringToWString(url).c_str(),
            NULL,
            0,
            INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID,
            0
        );
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return false;
        }

        std::ofstream outFile(outputFilePath, std::ios::binary);
        if (!outFile.is_open()) {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return false;
        }

        DWORD totalBytes = 0;
        DWORD headerLength = sizeof(totalBytes);
        HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &totalBytes, &headerLength, NULL);

        char buffer[1024];
        DWORD bytesRead;
        DWORD totalDownloaded = 0;

        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            outFile.write(buffer, bytesRead);
            totalDownloaded += bytesRead;

            if (totalBytes > 0) {
                double progress = (static_cast<double>(totalDownloaded) / totalBytes) * 100;
                progress_callback(progress);
            }
            else {
                progress_callback(-1);
            }
        }

        outFile.close();
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return true;
    }
}
