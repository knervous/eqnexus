export module http;

import <Windows.h>;
import <numeric>;
#include <wininet.h>
import <string>;
import <fstream>;
import <iostream>;
import <functional>;

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

    // Download binary ZIP data to a file with progress callback
    bool DownloadBinary(const std::string& url, const std::string& outputFilePath, std::function<void(double)> progress_callback) {
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

        // Get the total file size
        DWORD totalBytes = 0;
        DWORD bytesAvailable = 0;
        if (!InternetQueryDataAvailable(hConnect, &bytesAvailable, 0, 0)) {
            totalBytes = 0; // Unknown total size
        }
        else {
            totalBytes = bytesAvailable;
        }

        char buffer[1024];
        DWORD bytesRead;
        DWORD totalDownloaded = 0;

        // Read and write data in chunks, calling the callback with progress
        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            outFile.write(buffer, bytesRead);
            totalDownloaded += bytesRead;

            // Calculate progress and call the callback
            if (totalBytes > 0) {
                double progress = (static_cast<double>(totalDownloaded) / totalBytes) * 100;
                progress_callback(progress);
            }
            else {
                // If total size is unknown, call callback with -1 (or skip this condition if not needed)
                progress_callback(-1);
            }
        }

        outFile.close();
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return true;
    }
}
