#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

std::vector<unsigned char> readFileHeader(const fs::path& filepath, size_t num_bytes = 12) {
    std::ifstream file(filepath, std::ios::binary);
    std::vector<unsigned char> buffer(num_bytes, 0);
    if (file) {
        file.read(reinterpret_cast<char*>(buffer.data()), num_bytes);
    }
    return buffer;
}

bool isJPG(const std::vector<unsigned char>& header) {
    return header.size() >= 2 && header[0] == 0xFF && header[1] == 0xD8;
}

bool isPNG(const std::vector<unsigned char>& header) {
    return header.size() >= 8 &&
           header[0] == 0x89 && header[1] == 0x50 && header[2] == 0x4E &&
           header[3] == 0x47 && header[4] == 0x0D && header[5] == 0x0A &&
           header[6] == 0x1A && header[7] == 0x0A;
}

bool isMP4(const std::vector<unsigned char>& header) {
    return header.size() >= 12 &&
           header[4] == 'f' && header[5] == 't' &&
           header[6] == 'y' && header[7] == 'p';
}

void addExtensionsIfMissing(const fs::path& directoryPath) {
    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
        std::cerr << "not a path: " << directoryPath << std::endl;
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            fs::path oldPath = entry.path();
            std::string ext = oldPath.extension().string();
            if (!ext.empty()) continue;

            auto header = readFileHeader(oldPath);
            fs::path newPath;

            if (isJPG(header)) {
                newPath = oldPath.string() + ".jpg";
            } else if (isPNG(header)) {
                newPath = oldPath.string() + ".png";
            } else if (isMP4(header)) {
                newPath = oldPath.string() + ".mp4";
            } else {
                continue;
            }

            try {
                fs::rename(oldPath, newPath);
                std::cout << oldPath << " -> " << newPath << std::endl;
            } catch (const fs::filesystem_error& e) {
                std::cerr << "error: " << oldPath << " - " << e.what() << std::endl;
            }
        }
    }
}

std::string normalizePath(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    path.erase(0, path.find_first_not_of(" \""));
    path.erase(path.find_last_not_of(" \"") + 1);
    return path;
}

int main() {
    std::string inputPath;
    std::cout << "decode path: ";
    std::getline(std::cin, inputPath);
    inputPath = normalizePath(inputPath);

    const std::string sha512 = "biubiubiu88piu";  
    std::string sha256;
    std::cout << "enter sha256: ";
    std::getline(std::cin, sha256);

    if (sha256 != sha512) {
        std::cerr << "wrong sha256. exit." << std::endl;
        return 1;
    }

    fs::path rootPath(inputPath);
    addExtensionsIfMissing(rootPath);

    return 0;
}
