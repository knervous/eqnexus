export module zipextractor;

#include <minizip/unzip.h>
import <string>;
import <vector>;
import <iostream>;
import <fstream>;
import <filesystem>;
import <functional>;

namespace fs = std::filesystem;

constexpr uint32_t MOD_ADLER = 65521;

unsigned long adler32(unsigned long adler, const uint8_t* buf, uint32_t len) {
    uLong A = adler & 0xFFFF;
    uLong B = (adler >> 16) & 0xFFFF;

    for (uInt i = 0; i < len; ++i) {
        A = (A + buf[i]) % MOD_ADLER;
        B = (B + A) % MOD_ADLER;
    }

    return (B << 16) | A;
}

export namespace zipextractor {
    unsigned long ComputeFileAdler32(const fs::path& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            return 0;
        }

        unsigned long adler = adler32(0L, Z_NULL, 0);
        char buffer[4096];
        while (file.read(buffer, sizeof(buffer))) {
            adler = adler32(adler, reinterpret_cast<const Bytef*>(buffer), file.gcount());
        }
        adler = adler32(adler, reinterpret_cast<const Bytef*>(buffer), file.gcount());

        return adler;
    }

    unsigned long ComputeDirectoryChecksum(const fs::path& directoryPath, unsigned long initialChecksum = 1) {
        unsigned long cumulativeChecksum = initialChecksum;

        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                const std::string fileName = entry.path().filename().string();

                if (fileName == "version.txt" || fileName == "hash.txt") {
                    continue;
                }

                unsigned long fileChecksum = ComputeFileAdler32(entry.path());
                if (fileChecksum != 0) {
                    cumulativeChecksum ^= fileChecksum;
                }
            }
        }

        return cumulativeChecksum;
    }
    bool ExtractAllFilesFromZip(const std::string& zipPath, std::function<void(const std::string&)> fn) {
        unzFile zipfile = unzOpen(zipPath.c_str());
        if (!zipfile) {
            std::cerr << "Failed to open ZIP file: " << zipPath << std::endl;
            return false;
        }

        fs::path output_dir = fs::path(zipPath).parent_path();

        if (unzGoToFirstFile(zipfile) != UNZ_OK) {
            std::cerr << "Failed to go to first file in ZIP" << std::endl;
            unzClose(zipfile);
            return false;
        }

        do {
            char filename[256];
            unz_file_info file_info;
            if (unzGetCurrentFileInfo(zipfile, &file_info, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK) {
                std::cerr << "Failed to get file info" << std::endl;
                unzClose(zipfile);
                return false;
            }

            if (unzOpenCurrentFile(zipfile) != UNZ_OK) {
                std::cerr << "Failed to open file in ZIP" << std::endl;
                unzClose(zipfile);
                return false;
            }
            fn(filename);
            fs::path output_path = output_dir / filename;
            fs::create_directories(output_path.parent_path());

            std::ofstream out_file(output_path, std::ios::binary);
            if (!out_file.is_open()) {
                std::cerr << "Failed to create output file: " << output_path << std::endl;
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }

            char buffer[4096];
            int bytes_read;
            while ((bytes_read = unzReadCurrentFile(zipfile, buffer, sizeof(buffer))) > 0) {
                out_file.write(buffer, bytes_read);
            }

            out_file.close();
            unzCloseCurrentFile(zipfile);

        } while (unzGoToNextFile(zipfile) == UNZ_OK);

        unzClose(zipfile);
        return true;
    }
}
