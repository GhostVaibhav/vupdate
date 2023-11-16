#include "vlib/vlib.h"

#include "json.hpp"
#include "vconsolid/vconsolid.h"
#include "vdownload/vdownload.h"

void vupdate::update() {
  try {
    // Hiding the cursor
    indicators::show_console_cursor(false);

    // Get the filelist.json from the server
    getFile(fileServer, "./filelist.json", {}, spaceFiller, showProgress, port);

    // Opening the filelist.json
    std::ifstream filelist("filelist.json");
    if (!filelist.good()) {
      throw std::runtime_error("filelist.json not found on the server");
    }

    // Parse the filelist.json
    nlohmann::json filelistJson;
    filelist >> filelistJson;

    // Get the local filelist.json
    std::ifstream localFilelist("localFilelist.json");
    nlohmann::json localFilelistJson;
    if (localFilelist.good()) {
      try {
        localFilelist >> localFilelistJson;
      } catch (std::exception& e) {
        throw e;
      }
    }
    localFilelist.close();

    indicators::ProgressBar bar, bar2;

    if (showProgress) {
      bar.set_option(indicators::option::BarWidth{30});
      bar2.set_option(indicators::option::BarWidth{30});
      bar.set_option(indicators::option::Start{"["});
      bar2.set_option(indicators::option::Start{"["});
      bar.set_option(indicators::option::Fill{"="});
      bar2.set_option(indicators::option::Fill{"="});
      bar.set_option(indicators::option::Lead{">"});
      bar2.set_option(indicators::option::Lead{">"});
      bar.set_option(indicators::option::End{"]"});
      bar2.set_option(indicators::option::End{"]"});
      bar.set_option(indicators::option::ShowPercentage{true});
      bar2.set_option(indicators::option::ShowPercentage{true});
      bar.set_option(
          indicators::option::PostfixText{" | Verifying and downloading"});
      bar2.set_option(indicators::option::PostfixText{" | Verifying"});
      bar.set_option(
          indicators::option::MaxProgress{filelistJson["files"].size()});
      bar2.set_option(
          indicators::option::MaxProgress{filelistJson["files"].size()});
    } else {
      bar.set_option(indicators::option::BarWidth{0});
      bar2.set_option(indicators::option::BarWidth{0});
    }

    // Compare the two filelists
    if (sha256_file((char*)"./filelist.json", {}) !=
        sha256_file((char*)"./localFilelist.json", {})) {
      for (auto& [key, value] : filelistJson["files"].items()) {
        if (value == sha256_file((char*)(key.c_str()), {})) {
          std::ofstream localFilelist("localFilelist.json");
          localFilelistJson["files"][key] = value;
          localFilelist << localFilelistJson;
          localFilelist.close();
        } else if (localFilelistJson["files"][key].is_null() ||
                   localFilelistJson["files"][key] != value) {
          // File not found in local filelist
          getFile(fileServer, key, {}, {}, showProgress, port);
          consolid(key, localFilelistJson, {});
          localFilelistJson.clear();
          try {
            std::ifstream localFilelist("localFilelist.json");
            localFilelist >> localFilelistJson;
            localFilelist.close();
          } catch (...) {
            throw std::runtime_error("Error reading the local filelist");
          }
        }
        if (showProgress) bar.tick();
      }
    }

    if (!skipVerify) {
      // Verify the two filelists
      for (auto& [key, value] : filelistJson["files"].items()) {
        if (sha256_file((char*)(key.c_str()), {}) != value) {
          // File not found in local filelist
          getFile(fileServer, key, {}, {}, showProgress, port);
          consolid(key, localFilelistJson, {});
          localFilelistJson.clear();
          try {
            std::ifstream localFilelist("localFilelist.json");
            localFilelist >> localFilelistJson;
            localFilelist.close();
          } catch (...) {
            throw std::runtime_error("Error reading the local filelist");
          }
        }
        if (showProgress) bar2.tick();
      }
    }

    // Delete the files which are not in the server filelist
    for (auto& [key, value] : localFilelistJson["files"].items()) {
      if (filelistJson["files"].find(key) == filelistJson["files"].end()) {
        // File not found in server filelist
        std::remove(key.c_str());
      }
    }

    // Close all the files
    filelist.close();

    // Rename the filelist.json to localFilelist.json and remove the old
    // localFilelist.json
    try {
      std::remove("localFilelist.json");
      std::rename("filelist.json", "localFilelist.json");
    } catch (std::exception& e) {
      throw e;
    }

    // Showing the cursor
    indicators::show_console_cursor(true);
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    indicators::show_console_cursor(true);
  }
}

void vupdate::setServer(std::string _server) { fileServer = _server; }

void vupdate::setPort(unsigned int _port) { port = _port; }

void vupdate::setFiller(std::string _filler) { spaceFiller = _filler; }

void vupdate::setProgress(bool _progress) { showProgress = _progress; }

void vupdate::setSkipVerify(bool _verify) { skipVerify = _verify; }

void vupdate::setJson(bool _json) { readFromJson = _json; }
