export module zipextractor;

import utility;
import threadpool;

#include <minizip/unzip.h>
import <string>;
import <vector>;
import <iostream>;
import <fstream>;
import <xxhash.h>;
import <filesystem>;
import <functional>;
import <chrono>;

namespace fs = std::filesystem;

export namespace zipextractor
{
bool
ProcessZipFileWithHash(const std::string& zipPath,
                       std::function<void(const std::string& filePath, const std::string& xxhash3Hash)> callback)
{
    unzFile zipfile = unzOpen(zipPath.c_str());
    if (!zipfile)
    {
        std::cerr << "Failed to open ZIP file: " << zipPath << std::endl;
        return false;
    }

    if (unzGoToFirstFile(zipfile) != UNZ_OK)
    {
        std::cerr << "Failed to go to the first file in ZIP" << std::endl;
        unzClose(zipfile);
        return false;
    }

    do
    {
        char filename[256];
        unz_file_info file_info;
        if (unzGetCurrentFileInfo(zipfile, &file_info, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            std::cerr << "Failed to get file info" << std::endl;
            unzClose(zipfile);
            return false;
        }

        if (unzOpenCurrentFile(zipfile) != UNZ_OK)
        {
            std::cerr << "Failed to open file in ZIP" << std::endl;
            unzClose(zipfile);
            return false;
        }

        auto* state = XXH64_createState();
        if (!state)
        {
            std::cerr << "Failed to create xxHash3 state" << std::endl;
            unzClose(zipfile);
            return false;
        }
        XXH64_reset(state, 0);

        // Read the uncompressed file in chunks and update the hash
        char buffer[4096];
        int bytesRead;
        while ((bytesRead = unzReadCurrentFile(zipfile, buffer, sizeof(buffer))) > 0)
        {
            XXH64_update(state, buffer, bytesRead);
        }

        unzCloseCurrentFile(zipfile);

        if (bytesRead < 0)
        {
            std::cerr << "Error reading file in ZIP: " << filename << std::endl;
            XXH64_freeState(state);
            unzClose(zipfile);
            return false;
        }

        XXH64_hash_t hash = XXH64_digest(state);
        XXH64_freeState(state);

        std::ostringstream oss;
        oss << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << hash;
        std::string xxhash3Hex = oss.str();

        callback(filename, xxhash3Hex);

    } while (unzGoToNextFile(zipfile) == UNZ_OK);

    unzClose(zipfile);
    return true;
}

bool
ExtractAllFilesFromZip(const std::string& zipPath, std::function<void(const std::string&)> fn = nullptr)
{
    unzFile zipfile = unzOpen(zipPath.c_str());
    if (!zipfile)
    {
        std::cerr << "Failed to open ZIP file: " << zipPath << std::endl;
        return false;
    }

    fs::path output_dir = fs::path(zipPath).parent_path();

    if (unzGoToFirstFile(zipfile) != UNZ_OK)
    {
        std::cerr << "Failed to go to first file in ZIP" << std::endl;
        unzClose(zipfile);
        return false;
    }

    do
    {
        char filename[256];
        unz_file_info file_info;
        if (unzGetCurrentFileInfo(zipfile, &file_info, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            std::cerr << "Failed to get file info" << std::endl;
            unzClose(zipfile);
            return false;
        }

        if (unzOpenCurrentFile(zipfile) != UNZ_OK)
        {
            std::cerr << "Failed to open file in ZIP" << std::endl;
            unzClose(zipfile);
            return false;
        }
        if (fn)
        {
            fn(filename); 
        }
        fs::path output_path = output_dir / filename;
        fs::create_directories(output_path.parent_path());

        std::ofstream out_file(output_path, std::ios::binary);
        if (!out_file.is_open())
        {
            std::cerr << "Failed to create output file: " << output_path << std::endl;
            unzCloseCurrentFile(zipfile);
            unzClose(zipfile);
            return false;
        }

        char buffer[4096];
        int bytes_read;
        while ((bytes_read = unzReadCurrentFile(zipfile, buffer, sizeof(buffer))) > 0)
        {
            out_file.write(buffer, bytes_read);
        }

        out_file.close();
        unzCloseCurrentFile(zipfile);

    } while (unzGoToNextFile(zipfile) == UNZ_OK);

    unzClose(zipfile);
    return true;
}
}  // namespace zipextractor
