#include <filesystem>
#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"
#include "openssl/sha.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "vconsolid/vconsolid.h"

int main() {
  // Initializing the logger
  std::shared_ptr<spdlog::logger> logger =
      spdlog::basic_logger_mt<spdlog::async_factory>("vpack_logger", "log.txt");
  logger->info("Starting vpack");

  // Opening the filelist.json
  std::ofstream file("filelist.json");
  nlohmann::json filelist;

  // Compute all the hashes of the files in the current directory
  for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
    if (entry.is_regular_file() &&
        entry.path().string() != ".\\filelist.json" &&
        entry.path().string() != ".\\vpack" &&
        entry.path().string() != ".\\vpack.exe" &&
        entry.path().string() != ".\\log.txt") {
      std::string path = entry.path().generic_string();
      filelist["files"][path] = sha256_file((char*)path.c_str(), logger);
      std::cout << path << std::endl;
    }
  }

  // Write the filelist.json
  file << filelist.dump();

  // Closing the file
  file.close();

  return 0;
}