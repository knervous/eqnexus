export module server_info;

import globals;
import http;
import zipextractor;
import utility;
import login;
import nk;
import login_frontend;
import threadpool;

import "nuklear_d3d9.h";
import <windows.h>;
import <string>;
import <functional>;
import <vector>;
import <thread>;
import <filesystem>;
import <iostream>;
import <fstream>;
import <chrono>;
import <memory>;
import <rapidjson/document.h>;
import <rapidjson/prettywriter.h>;
import <rapidjson/stringbuffer.h>;
import <rapidjson/ostreamwrapper.h>;

namespace fs = std::filesystem;

export class ServerInfo
{
   public:
    ServerInfo(const std::string& shortname                       = "",
               const std::string& longname                        = "",
               const std::string& url                             = "",
               const std::string& filesUrlPrefix                  = "",
               const std::string& version                         = "",
               const std::string& website                         = "",
               const std::string& description                     = "",
               std::vector<std::string> hosts                     = {},
               std::vector<std::string> required                  = {},
               std::unordered_map<std::string, std::string> files = {})
        : shortName(shortname)
        , longName(longname)
        , filesUrlPrefix(filesUrlPrefix)
        , customFilesUrl(url)
        , version(version)
        , website(website)
        , description(description)
        , hosts(hosts)
        , required(required)
        , files(files)
    {
    }

    ~ServerInfo()
    {
        if (validation_thread.joinable())
        {
            validation_thread.join();
        }
    }

    ServerInfo(ServerInfo&& other) noexcept
        : shortName(std::move(other.shortName))
        , longName(std::move(other.longName))
        , customFilesUrl(std::move(other.customFilesUrl))
        , version(std::move(other.version))
        , website(std::move(other.website))
        , description(std::move(other.description))
        , hosts(std::move(other.hosts))
        , required(std::move(other.required))
        , files(std::move(other.files))
        , up_to_date(other.up_to_date)
        , downloading(other.downloading)
        , ran_validation(other.ran_validation)
        , running_validation(other.running_validation)
        , filesUrlPrefix(other.filesUrlPrefix)
        , error(std::move(other.error))
        , pct_downloaded(other.pct_downloaded)
        , status(std::move(other.status))
        , validation_thread(std::move(other.validation_thread))
    {
        other.up_to_date         = false;
        other.downloading        = false;
        other.ran_validation     = false;
        other.running_validation = false;
        other.pct_downloaded     = 0.0;
    }

    ServerInfo& operator=(ServerInfo&& other) noexcept
    {
        if (this != &other)
        {
            if (validation_thread.joinable())
            {
                validation_thread.join();
            }

            validation_thread = std::move(other.validation_thread);
        }
        return *this;
    }

    ServerInfo(const ServerInfo&)            = delete;
    ServerInfo& operator=(const ServerInfo&) = delete;

    const std::vector<std::string> GetHosts()
    {
        return hosts;
    }

    void AddFileCRC(const std::string& file, const std::string& crc)
    {
        files[file] = crc;
    }

    const eqlib::EQClientServerData* GetClientServer()
    {
        return clientServer;
    }

    const std::string GetLongName()
    {
        return longName;
    }

    const std::string GetShortName()
    {
        return shortName;
    }

    const bool UpToDate()
    {
        return up_to_date;
    }

    const bool DidValidate()
    {
        return ran_validation;
    }

    const bool IsValidating()
    {
        return running_validation;
    }

    void ValidateInstallAsync(bool blocking = false)
    {
        if (blocking)
        {
            bool result           = true;
            const auto& root_path = "eqnexus/" + shortName;
            fs::path path(root_path);

            for (const auto& [file, hash] : files)
            {
                for (const auto& required_file : required)
                {
                    if (file == required_file)
                    {
                        auto filePath = path / file;
                        if (!fs::exists(filePath))
                        {
                            result = false;
                            break;
                        }

                        auto fileHash = util::GenerateFileHash(filePath.string());
                        if (fileHash != hash)
                        {
                            result = false;
                            break;
                        }
                    }
                }
            }
            up_to_date     = result;
            ran_validation = true;
            return;
        }

        if (running_validation)
            return;

        running_validation = true;

        auto validation_task = [this]() mutable {
            const auto& root_path = "eqnexus/" + shortName;
            fs::path version_path = root_path + "/version.txt";
            validation_status.setString("Checking patch files...");
            bool result = true;
            std::string stored_version;
            std::ifstream version_file(version_path);
            version_file >> stored_version;

            if (!fs::exists(root_path) || !fs::exists(version_path) || version != stored_version)
            {
                result = false;
                for (const auto& pair : files)
                {
                    outOfDateFiles.push_back(pair.first);
                }
            }
            else
            {
                if (files.size() > 0)
                {
                    for (const auto& [file, hash] : files)
                    {
                        fs::path path(root_path);
                        validation_status.setString(util::Interpolate("Checking {}", util::ExtractFilename(file)));
                        auto filePath = path / file;
                        if (!fs::exists(filePath))
                        {
                            result = false;
                            outOfDateFiles.push_back(file);
                        }

                        auto fileHash = util::GenerateFileHash(filePath.string());
                        if (fileHash != hash)
                        {
                            result = false;
                            outOfDateFiles.push_back(file);
                        }
                    }
                }
            }

            up_to_date         = result;
            ran_validation     = true;
            running_validation = false;
        };

        validation_thread = std::thread([validation_task = std::move(validation_task)]() mutable { validation_task(); });
        validation_thread.detach();
    }

    void DrawListItem(nk_context* ctx, bool& task_running)
    {
        CheckClientServer();
        nk_layout_row_dynamic(ctx, 20, 2);
        if (nk_widget_is_hovered(ctx))
        {
            nk_tooltip(ctx, longName.c_str());
        }
        nk_label(ctx, nk_util::TruncateTextWithEllipsis(ctx, longName.c_str(), 200.0f).c_str(), NK_TEXT_LEFT);
        if (nk_button_label(ctx, "Server Info"))
        {
            Login::OpenModal(util::Interpolate(R"(
                        Server: {}<br></br>
                        Version: {}<br></br>
                        Website: <a href="{}">{}</a><br></br>
                        Description: {}
                    )",
                                               longName,
                                               version,
                                               website,
                                               website,
                                               description));
        }

        bool server_task_running = !status.empty() || downloading;

        if (downloading)
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx,
                     util::Interpolate("Downloading: {} ({}/{})", download_file, download_files, download_files_total).c_str(),
                     NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, util::Interpolate("{}% complete", util::ToStringWithPrecision(pct_downloaded)).c_str(), NK_TEXT_CENTERED);
        }

        if (running_validation)
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, validation_status.getString().c_str(), NK_TEXT_LEFT);
        }
        else if (!ran_validation)
        {
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_label(ctx, "Needs Check", NK_TEXT_LEFT);
            if (nk_button_label(ctx, "Check For Updates"))
            {
                ValidateInstallAsync();
            }
        }
        else if (!up_to_date)
        {
            if (!task_running)
            {
                nk_layout_row_dynamic(ctx, 25, 2);
                nk_label(ctx, "Needs Update", NK_TEXT_LEFT);

                if (nk_button_label(ctx, "Download"))
                {
                    DownloadServerFiles(task_running);
                }
            }
        }
        else if (up_to_date)
        {
            nk_layout_row_dynamic(ctx, 25, 4);
            nk_label(ctx, "Up To Date", NK_TEXT_LEFT);
            nk_label(ctx, "", NK_TEXT_LEFT);

            if (nk_button_label(ctx, "Files"))
            {
                util::OpenExplorerInCustomDirectory(shortName);
            }

            if (clientServer)
            {
                if (nk_button_label(ctx, "Play"))
                {
                    Login::LoginToServer(clientServer->ID);
                }
            }
            else
            {
                nk_util::RenderDisabledButton(ctx, "Offline");
            }
        }
        nk_util::DrawSeparator(ctx);
    }

    bool SerializeManifest(const std::string& filename)
    {
        using namespace rapidjson;
        Document document;
        document.SetObject();
        Document::AllocatorType& allocator = document.GetAllocator();

        // Add each attribute to the JSON document
        document.AddMember("shortName", Value(shortName.c_str(), allocator), allocator);
        document.AddMember("longName", Value(longName.c_str(), allocator), allocator);
        document.AddMember("customFilesUrl", Value(customFilesUrl.c_str(), allocator), allocator);
        document.AddMember("filesUrlPrefix", Value(filesUrlPrefix.c_str(), allocator), allocator);
        document.AddMember("version", Value(version.c_str(), allocator), allocator);
        document.AddMember("website", Value(website.c_str(), allocator), allocator);
        document.AddMember("description", Value(description.c_str(), allocator), allocator);

        Value hostsArray(kArrayType);
        for (const auto& host : hosts)
        {
            hostsArray.PushBack(Value(host.c_str(), allocator), allocator);
        }
        document.AddMember("hosts", hostsArray, allocator);

        Value requiredArray(kArrayType);
        for (const auto& required_file : required)
        {
            requiredArray.PushBack(Value(required_file.c_str(), allocator), allocator);
        }
        document.AddMember("required", requiredArray, allocator);

        Value filesObject(kObjectType);
        for (const auto& file : files)
        {
            filesObject.AddMember(Value(file.first.c_str(), allocator), Value(file.second.c_str(), allocator), allocator);
        }
        document.AddMember("files", filesObject, allocator);

        // Open a file stream
        std::ofstream ofs(filename);
        if (!ofs.is_open())
        {
            return false;
        }

        // Write JSON directly to the file
        OStreamWrapper osw(ofs);
        PrettyWriter<OStreamWrapper> writer(osw);
        document.Accept(writer);

        ofs.close();
        return true;
    }

    void CreateManifest(std::function<void(const std::string&)> cb)
    {
        auto shortNameCopy      = shortName;
        auto longNameCopy       = longName;
        auto customFilesUrlCopy = customFilesUrl;
        auto filesUrlPrefixCopy = filesUrlPrefix;
        auto versionCopy        = version;
        auto websiteCopy        = website;
        auto descriptionCopy    = description;
        auto hostsCopy          = hosts;
        auto filesCopy          = files;

        std::thread t([this,
                       cb,
                       shortNameCopy,
                       longNameCopy,
                       customFilesUrlCopy,
                       filesUrlPrefixCopy,
                       versionCopy,
                       websiteCopy,
                       descriptionCopy,
                       hostsCopy,
                       filesCopy]() {
            auto server = ServerInfo(shortNameCopy,
                                     longNameCopy,
                                     customFilesUrlCopy,
                                     filesUrlPrefixCopy,
                                     versionCopy,
                                     websiteCopy,
                                     descriptionCopy,
                                     hostsCopy,
                                     {});
            auto file   = util::OpenFileDialog();
            if (!file.empty())
            {
                zipextractor::ProcessZipFileWithHash(file, [&server, &cb](const std::string& name, const std::string& hash) {
                    server.AddFileCRC(name, hash);
                    cb(std::move(name));
                });
                cb("");
                util::ReplaceAll(file, "zip", "json");
                fs::path filePath(file);
                const fs::path outputPath =
                    filePath.parent_path() /
                    (server.GetShortName().empty() ? "generated.json" : ("generated_" + server.GetShortName() + ".json"));
                fs::remove(outputPath);
                auto outPath = outputPath.string();
                if (server.SerializeManifest(outPath))
                {
                    Login::OpenModal(
                        util::Interpolate("Successfully created server manifest JSON file: <a "
                                          "href=\"file:///{}\" target=\"_blank\">{}</a>",
                                          outPath,
                                          outPath));
                }
                else
                {
                    Login::OpenModal("Error generating and writing file " + outPath);
                }
            }
        });
        t.detach();
    }

   private:
    // Manifest attributes
    std::string shortName                              = "";
    std::string longName                               = "";
    std::string customFilesUrl                         = "";
    std::string filesUrlPrefix                         = "";
    std::string version                                = "";
    std::string website                                = "";
    std::string description                            = "";
    std::vector<std::string> hosts                     = {};
    std::vector<std::string> required                  = {};
    std::vector<std::string> outOfDateFiles            = {};
    std::unordered_map<std::string, std::string> files = {};

    // Lifetime attributes
    eqlib::EQClientServerData* clientServer = nullptr;
    bool up_to_date                         = false;
    bool downloading                        = false;
    bool ran_validation                     = false;
    bool running_validation                 = false;
    int download_files                      = 0;
    int download_files_total                = 0;
    std::string download_file               = "";
    std::thread validation_thread;
    util::AtomicString validation_status;
    std::string error     = "";
    double pct_downloaded = 0.0;
    std::string status    = "";

    void WriteHashAndVersion()
    {
        const auto& root_path = "eqnexus/" + shortName;
        fs::path version_path = root_path + "/version.txt";
        std::ofstream version_out(version_path, std::ios::trunc);
        version_out << version;
    }

    void CheckClientServer()
    {
        if (clientServer)
        {
            return;
        }
        for (auto& s : eqlib::g_pLoginClient->ServerList)
        {
            for (const auto& host : hosts)
            {
                if (s->HostName.c_str() == host)
                {
                    clientServer = s;
                    break;
                }
            }
        }
    }

    void DownloadServerFilesTask(bool& task_running)
    {
        if (customFilesUrl.empty() && files.size() == 0)
        {
            Login::OpenModal("No Custom Zip URL defined");
            task_running = false;
            return;
        }

        if (files.size() > 0)
        {
            if (filesUrlPrefix.empty())
            {
                error        = "No URL defined";
                task_running = false;
                Login::OpenModal("No URL prefix defined");
                return;
            }
        }

        const auto path = "eqnexus/" + shortName;
        bool exists     = fs::exists(path);
        if (!exists && !fs::create_directories(path))
        {
            Login::OpenModal("Filesystem Error. Unable to create directory: " + path);
            task_running = false;
            return;
        }

        // Download whole package if directory doesn't exist yet
        if (!exists)
        {
            downloading          = true;
            download_files       = 1;
            auto zip_path        = path + "/" + shortName + ".zip";
            download_files_total = 1;
            download_file        = util::ExtractFilename(customFilesUrl);
            if (http::DownloadBinary(customFilesUrl, zip_path, [this](double pct) { pct_downloaded = pct; }))
            {
                downloading = false;

                if (zipextractor::ExtractAllFilesFromZip(zip_path,
                                                         [this](const std::string& filename) { status = "Extracting " + filename; }) &&
                    fs::remove(zip_path))
                {
                    WriteHashAndVersion();
                }
                else
                {
                    Login::OpenModal("Extract Error. Error extracting zip from: " + zip_path);
                }
            }
            else
            {
                error = "HTTP Failure";
            }
        }
        else
        {
            downloading          = true;
            download_files_total = outOfDateFiles.size();

            ThreadPool pool(std::thread::hardware_concurrency());
            for (const auto& file : outOfDateFiles)
            {
                pool.enqueue([this, file, path]() {
                    const auto& file_path = path + "/" + file;
                    const auto& url       = filesUrlPrefix + "/" + file;
                    http::DownloadBinary(url, file_path);
                    download_files++;
                    pct_downloaded = 100 * (static_cast<double>(download_files) / static_cast<double>(download_files_total));
                });
            }
            pool.wait();
            WriteHashAndVersion();
        }

        status               = "";
        downloading          = false;
        download_files       = 0;
        download_files_total = 0;
        download_file        = "";
        pct_downloaded       = 0.0;
        outOfDateFiles.clear();
        ValidateInstallAsync();
        task_running = false;
    }

    inline void DownloadServerFiles(bool& task_running)
    {
        task_running = true;
        std::thread(&ServerInfo::DownloadServerFilesTask, this, std::ref(task_running)).detach();
    }
};
