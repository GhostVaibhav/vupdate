#include <fstream>
#include <iostream>
#include <filesystem>

#include "openssl/sha.h"
#include "nlohmann/json.hpp"

#include "vconsolid/vconsolid.h"

int main() {
    // Opening the filelist.json
    std::ofstream file("filelist.json");
    nlohmann::json filelist;

    // Compute all the hashes of the files in the current directory
    for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
        if (entry.is_regular_file() && entry.path().string() != ".\\filelist.json" && entry.path().string() != ".\\vpack" && entry.path().string() != ".\\vpack.exe") {
            std::string path = entry.path().generic_string();
            filelist["files"][path] = sha256_file((char*) path.c_str());
            std::cout << path << std::endl;
        }
    }

    // Write the filelist.json
    file << filelist.dump();

    // Closing the file
    file.close();

    return 0;
}