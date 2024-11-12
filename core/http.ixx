export module http;

import <Windows.h>;
import <string>;
import <fstream>;
import <iostream>;
import <functional>;
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

constexpr auto* USER_AGENT        = L"EQNexus";
constexpr DWORD CALLBACK_INTERVAL = 1024 * 100;  // 100 KB

std::wstring
StringToWString(const std::string& str)
{
    return std::wstring(str.begin(), str.end());
}

export namespace http
{

// Utility function to extract hostname, path, and determine if URL is HTTPS
bool
ParseUrl(const std::wstring& url, std::wstring& server, std::wstring& path, bool& isHttps)
{
    URL_COMPONENTS urlComp   = {sizeof(urlComp)};
    urlComp.dwSchemeLength   = 1;
    urlComp.dwHostNameLength = 1;
    urlComp.dwUrlPathLength  = 1;

    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &urlComp))
        return false;

    server.assign(urlComp.lpszHostName, urlComp.dwHostNameLength);
    path.assign(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
    isHttps = (urlComp.nScheme == INTERNET_SCHEME_HTTPS);
    return true;
}

std::string
DownloadJson(const std::string& url)
{
    std::wstring wUrl = StringToWString(url);
    std::wstring server, path;
    bool isHttps;
    if (!ParseUrl(wUrl, server, path, isHttps))
        return "";

    HINTERNET hSession = WinHttpOpen(USER_AGENT,
                                     WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                     WINHTTP_NO_PROXY_NAME,
                                     WINHTTP_NO_PROXY_BYPASS,
                                     0);
    if (!hSession)
        return "";

    HINTERNET hConnect =
        WinHttpConnect(hSession,
                       server.c_str(),
                       isHttps ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT,
                       0);
    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return "";
    }

    DWORD requestFlags = isHttps ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
                                            L"GET",
                                            path.c_str(),
                                            NULL,
                                            WINHTTP_NO_REFERER,
                                            WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            requestFlags);
    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    std::string result;
    if (WinHttpSendRequest(
            hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, NULL))
    {
        DWORD bytesAvailable = 0;
        do
        {
            WinHttpQueryDataAvailable(hRequest, &bytesAvailable);
            if (bytesAvailable > 0)
            {
                char* buffer = new char[bytesAvailable + 1];
                ZeroMemory(buffer, bytesAvailable + 1);

                DWORD bytesRead = 0;
                WinHttpReadData(hRequest, buffer, bytesAvailable, &bytesRead);
                result.append(buffer, bytesRead);
                delete[] buffer;
            }
        } while (bytesAvailable > 0);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return result;
}

bool
DownloadBinary(const std::string& url,
               const std::string& outputFilePath,
               std::function<void(double)> progress_callback = nullptr)
{
    std::wstring wUrl = StringToWString(url);
    std::wstring server, path;
    bool isHttps;
    if (!ParseUrl(wUrl, server, path, isHttps))
        return false;

    HINTERNET hSession = WinHttpOpen(USER_AGENT,
                                     WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                     WINHTTP_NO_PROXY_NAME,
                                     WINHTTP_NO_PROXY_BYPASS,
                                     0);
    if (!hSession)
        return false;

    HINTERNET hConnect =
        WinHttpConnect(hSession,
                       server.c_str(),
                       isHttps ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT,
                       0);
    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD requestFlags = isHttps ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
                                            L"GET",
                                            path.c_str(),
                                            NULL,
                                            WINHTTP_NO_REFERER,
                                            WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            requestFlags);
    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    std::ofstream outFile(outputFilePath, std::ios::binary);
    if (!outFile.is_open())
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD totalBytes = 0;
    if (WinHttpSendRequest(
            hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, NULL))
    {
        DWORD headerLength = sizeof(totalBytes);
        WinHttpQueryHeaders(hRequest,
                            WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX,
                            &totalBytes,
                            &headerLength,
                            WINHTTP_NO_HEADER_INDEX);

        char buffer[8192 * 4];
        DWORD bytesRead = 0, totalDownloaded = 0;
        DWORD lastProgressUpdate = 0;

        while (WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
        {
            outFile.write(buffer, bytesRead);
            totalDownloaded += bytesRead;

            if (progress_callback && totalBytes > 0 &&
                totalDownloaded - lastProgressUpdate >= CALLBACK_INTERVAL)
            {
                double progress = (static_cast<double>(totalDownloaded) / totalBytes) * 100;
                progress_callback(progress);
                lastProgressUpdate = totalDownloaded;
            }
        }

        if (progress_callback && totalBytes > 0)
        {
            progress_callback(100.0);
        }
    }

    outFile.close();
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return true;
}

}  // namespace http
