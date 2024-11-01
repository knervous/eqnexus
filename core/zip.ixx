export module zipextractor;

#include <zlib.h>
import <string>;
import <vector>;
import <iostream>;
import <fstream>;
import <filesystem>;

namespace fs = std::filesystem;

struct ZipFileHeader {
    uint32_t signature;
    uint16_t version;
    uint16_t flags;
    uint16_t compression;
    uint16_t modTime;
    uint16_t modDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t fileNameLength;
    uint16_t extraFieldLength;
};

bool ReadZipFileHeader(std::ifstream& file, ZipFileHeader& header) {
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    return header.signature == 0x04034b50;  // ZIP local file header signature
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
    bool ExtractAllFilesFromZip(const std::string& zipPath) {
        fs::path outputDir = fs::path(zipPath).parent_path();
        std::ifstream file(zipPath, std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Failed to open ZIP file: " << zipPath << std::endl;
            return false;
        }

        while (true) {
            ZipFileHeader header;
            if (!ReadZipFileHeader(file, header)) {
                break;
            }

            std::vector<char> fileName(header.fileNameLength);
            file.read(fileName.data(), header.fileNameLength);
            std::string outputFileName(fileName.begin(), fileName.end());

            file.seekg(header.extraFieldLength, std::ios::cur);

            fs::path outputPath = outputDir / outputFileName;
            fs::create_directories(outputPath.parent_path());

            std::vector<char> compressedData(header.compressedSize);
            file.read(compressedData.data(), header.compressedSize);

            std::vector<char> decompressedData(header.uncompressedSize);
            if (header.compression == 0) {
                decompressedData = std::move(compressedData);
            }
            else if (header.compression == 8) {
                z_stream stream{};
                stream.next_in = reinterpret_cast<Bytef*>(compressedData.data());
                stream.avail_in = static_cast<uInt>(compressedData.size());
                stream.next_out = reinterpret_cast<Bytef*>(decompressedData.data());
                stream.avail_out = static_cast<uInt>(decompressedData.size());

                if (inflateInit2(&stream, -MAX_WBITS) != Z_OK) {
                    std::cerr << "Failed to initialize zlib decompression" << std::endl;
                    return false;
                }

                int result = inflate(&stream, Z_FINISH);
                inflateEnd(&stream);

                if (result != Z_STREAM_END) {
                    std::cerr << "Decompression failed for file: " << outputFileName << std::endl;
                    return false;
                }
            }
            else {
                std::cerr << "Unsupported compression method for file: " << outputFileName << std::endl;
                return false;
            }

            std::ofstream outFile(outputPath, std::ios::binary);
            if (!outFile.is_open()) {
                std::cerr << "Failed to create output file: " << outputPath << std::endl;
                return false;
            }
            outFile.write(decompressedData.data(), decompressedData.size());
            outFile.close();
        }

        return true;
    }
}
