export module updater;

import http;
import zipextractor;
import utility;
import login;

import <string>;
import <thread>;
import <filesystem>;

typedef bool (*UpdateCallback)(const char* folder);
namespace fs = std::filesystem;

export struct CoreVersion {
    std::string version;
    std::string filename;
};

export class CoreUpdater
{
   public:
    static void SetCallback(void* cb)
    {
        callback = (UpdateCallback) cb;
    }

    static void UpdateCore(const CoreVersion& core_version, const std::string& latest_prefix)
    {
        std::thread t([core_version, latest_prefix]() {
            const auto zip_url = latest_prefix + "/latest/download/" + core_version.filename;
            auto folder        = "eqnexus-" + core_version.version;
            if (fs::exists(folder)) {
                fs::remove_all(folder);
            }
            auto zipPath       = folder + "/" + core_version.filename;
            http::DownloadBinary(zip_url, zipPath);
            zipextractor::ExtractAllFilesFromZip(zipPath);
            fs::remove(zipPath);
            if (callback)
            {
                if (!callback(folder.c_str()))
                {
                    std::thread t([latest_prefix]() {
                        while (!Login::DidRetrieveServers())
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(500));
                        }
                        Login::OpenModal(
                            "There was an error updating automatically. <br></br><br></br>To download the latest core version, "
                            "please visit <a href=\"" +
                            latest_prefix + "\">" + latest_prefix + "</a>");
                    });
                    t.detach();
                }
            }
        });
        t.detach();
    }

   private:
    inline static UpdateCallback callback = nullptr;
};