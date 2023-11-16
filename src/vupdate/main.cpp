#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <unordered_map>

#include "nlohmann/json.hpp"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "vconsolid/vconsolid.h"
#include "vdownload/vdownload.h"

void set_title(std::string title) {
#ifdef _WIN32
  SetConsoleTitle(TEXT(title.c_str()));
#else
  std::cout << "\033]0;" << title << "\007";
#endif
}

int main() {
  indicators::show_console_cursor(false);
  try {
    // Initialize the logger
    std::shared_ptr<spdlog::logger> logger =
        spdlog::basic_logger_mt<spdlog::async_factory>("vupdater_logger",
                                                       "log.txt");
    logger->info("Starting vupdater");

    // Open the configuration file
    std::ifstream config("config.json");
    if (!config.good()) {
      logger->error("config.json was not found");
      throw std::exception("config.json not found");
    }

    // Parse the configuration file
    nlohmann::json configJson;
    config >> configJson;
    logger->info("Config loaded");

    // Get the server url from the config
    std::string server = configJson["server"];
    logger->info("Server: {}", server);

    // Get the filelist.json from the server
    getFile(server, "./filelist.json", logger, {}, true, 0);
    logger->info("Filelist downloaded");

    // Opening the filelist.json
    std::ifstream filelist("filelist.json");
    if (!filelist.good()) {
      logger->error("Can't find filelist.json on the server");
      throw std::exception("filelist.json not found on the server");
    }

    // Parse the filelist.json
    nlohmann::json filelistJson;
    filelist >> filelistJson;

    logger->info("Filelist map created");

    // Get the local filelist.json
    std::ifstream localFilelist("localFilelist.json");
    nlohmann::json localFilelistJson;
    if (localFilelist.good()) {
      try {
        localFilelist >> localFilelistJson;
        logger->info("Local filelist map created");
      } catch (...) {
        logger->info("Local filelist map corrupted, creating new");
      }
    } else {
      logger->info("Local filelist not found, creating new");
    }
    localFilelist.close();

    indicators::ProgressBar bar{
        indicators::option::BarWidth{30},
        indicators::option::Start{" ["},
        indicators::option::Fill{"="},
        indicators::option::Lead{">"},
        indicators::option::End{"]"},
        indicators::option::ShowPercentage{true},
        indicators::option::PostfixText{" | Verifying and downloading"},
        indicators::option::MaxProgress{filelistJson["files"].size()}};

    // Compare the two filelists
    if (sha256_file((char*)"./filelist.json", logger) !=
        sha256_file((char*)"./localFilelist.json", logger)) {
      for (auto& [key, value] : filelistJson["files"].items()) {
        if (value == sha256_file((char*)(key.c_str()), logger)) {
          std::ofstream localFilelist("localFilelist.json");
          localFilelistJson["files"][key] = value;
          localFilelist << localFilelistJson;
          localFilelist.close();
        } else if (localFilelistJson["files"][key].is_null() ||
                   localFilelistJson["files"][key] != value) {
          // File not found in local filelist
          getFile(server, key, logger, {}, true, 0);
          consolid(key, localFilelistJson, logger);
          localFilelistJson.clear();
          try {
            std::ifstream localFilelist("localFilelist.json");
            localFilelist >> localFilelistJson;
            localFilelist.close();
          } catch (...) {
            logger->error("Can't read the local filelist");
            throw std::exception("Error reading the local filelist");
          }
          logger->info("File {} downloaded", key);
        }
        bar.tick();
        set_title(
            "Verifying and downloading: " +
            std::to_string(int(ceil(static_cast<float>(bar.current()) /
                                    filelistJson["files"].size() * 100))) +
            "%");
      }
    }

    indicators::ProgressBar bar2{
        indicators::option::BarWidth{30},
        indicators::option::Start{" ["},
        indicators::option::Fill{"="},
        indicators::option::Lead{">"},
        indicators::option::End{"]"},
        indicators::option::ShowPercentage{true},
        indicators::option::PostfixText{" | Verifying"},
        indicators::option::MaxProgress{filelistJson["files"].size()}};

    // Verify the two filelists
    for (auto& [key, value] : filelistJson["files"].items()) {
      if (sha256_file((char*)(key.c_str()), logger) != value) {
        // File not found in local filelist
        getFile(server, key, logger, {}, true, 0);
        consolid(key, localFilelistJson, logger);
        localFilelistJson.clear();
        try {
          std::ifstream localFilelist("localFilelist.json");
          localFilelist >> localFilelistJson;
          localFilelist.close();
        } catch (...) {
          logger->error("Can't read the local filelist");
          throw std::exception("Error reading the local filelist");
        }
        logger->info("File {} downloaded", key);
      }
      bar2.tick();
      set_title("Verifying: " +
                std::to_string(int(ceil(static_cast<float>(bar2.current()) /
                                        filelistJson["files"].size() * 100))) +
                "%");
    }

    // Delete the files which are not in the server filelist
    for (auto& [key, value] : localFilelistJson["files"].items()) {
      if (filelistJson["files"].find(key) == filelistJson["files"].end()) {
        // File not found in server filelist
        std::remove(key.c_str());
      }
    }

    // Close all the files
    config.close();
    filelist.close();

    // Rename the filelist.json to localFilelist.json and remove the old
    // localFilelist.json
    try {
      std::remove("localFilelist.json");
      std::rename("filelist.json", "localFilelist.json");
    } catch (std::exception& e) {
      logger->error("No delete or rename permission");
      throw e;
    }

    // Logging a blank line
    logger->info("");
    indicators::show_console_cursor(true);
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    indicators::show_console_cursor(true);
  }

  return 0;
}
