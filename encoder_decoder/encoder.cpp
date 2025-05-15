#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

std::string normalizePath(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    path.erase(0, path.find_first_not_of(" \""));
    path.erase(path.find_last_not_of(" \"") + 1);
    return path;
}

void removeExtensions(const fs::path& directoryPath) {
    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
        std::cerr << "not a path" << directoryPath << std::endl;
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            fs::path oldPath = entry.path();
            fs::path newPath = oldPath.parent_path() / oldPath.stem(); 

            try {
                fs::rename(oldPath, newPath);
                std::cout << oldPath << " -> " << newPath << std::endl;
            } catch (const fs::filesystem_error& e) {
                std::cerr << "error:" << oldPath << " - " << e.what() << std::endl;
            }
        }
    }
}

int main() {
    std::string inputPath;
    std::cout << "encode path: ";
    std::getline(std::cin, inputPath);

    inputPath = normalizePath(inputPath);
    fs::path rootPath(inputPath);
    removeExtensions(rootPath);

    return 0;
}
