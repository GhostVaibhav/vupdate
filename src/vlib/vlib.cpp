#include "vlib/vlib.h"

#include "json.hpp"
#include "vconsolid/vconsolid.h"
#include "vdownload/vdownload.h"

void vupdate::update() {
  try {
    // Hiding the cursor
    indicators::show_console_cursor(false);

    if (readFromJson) {
      std::ifstream config("config.json");
      if (config.good()) {
        nlohmann::json configJson;
        try {
          config >> configJson;
          if (configJson.find("server") == configJson.end() ||
              configJson.find("port") == configJson.end())
            throw std::runtime_error("");
          if (configJson["server"].is_string())
            fileServer = {configJson["server"], configJson["server"]};
          else
            fileServer = {configJson["server"].at(0),
                          configJson["server"].at(1)};
          if (configJson["port"].is_number())
            port = {configJson["port"], configJson["port"]};
          else
            port = {configJson["port"].at(0), configJson["port"].at(1)};
          if (configJson.find("filler") != configJson.end())
            spaceFiller = configJson["filler"];
          if (configJson.find("progress") != configJson.end() &&
              configJson["progress"].is_boolean())
            showProgress = configJson["progress"].get<bool>();
          if (configJson.find("nVerify") != configJson.end() &&
              configJson["nVerify"].is_boolean())
            skipVerify = configJson["nVerify"].get<bool>();
        } catch (...) {
          throw std::runtime_error("Corrupted or missing config file");
        }
      } else {
        throw std::runtime_error("config file not found");
      }
    }

    // Get the filelist.json from the server
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || \
    defined(__CYGWIN__) || defined(_WIN64)
    getFile(fileServer[0], "./filelist.json", {}, spaceFiller, showProgress,
            port[0], progressCallback);
#else
    getFile(fileServer[1], "./filelist.json", {}, spaceFiller, showProgress,
            port[1], progressCallback);
#endif

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
      } catch (...) {
        std::ofstream ofs("localFileList.json",
                          std::ofstream::out | std::ofstream::trunc);
        ofs.close();
        localFilelistJson.clear();
      }
    }
    localFilelist.close();

    // Compare the two filelists
    if (sha256_file((char*)"./filelist.json", {}) !=
        sha256_file((char*)"./localFilelist.json", {})) {
      for (auto& [key, value] : filelistJson["files"].items()) {
        if (showProgress && progressCallback == nullptr) {
          indicators::ProgressBar bar{
              indicators::option::BarWidth{30},
              indicators::option::Start{"["},
              indicators::option::Fill{"="},
              indicators::option::Lead{">"},
              indicators::option::End{"]"},
              indicators::option::ShowPercentage{true},
              indicators::option::PostfixText{" | Verifying and downloading"},
              indicators::option::MaxProgress{filelistJson["files"].size()}};
          if (value == sha256_file((char*)(key.c_str()), {})) {
            std::ofstream localFilelist("localFilelist.json");
            localFilelistJson["files"][key] = value;
            localFilelist << localFilelistJson;
            localFilelist.close();
          } else if (localFilelistJson["files"][key].is_null() ||
                     localFilelistJson["files"][key] != value) {
            // File not found in local filelist
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || \
    defined(__CYGWIN__) || defined(_WIN64)
            getFile(fileServer[0], key, {}, {}, showProgress, port[0], {});
#else
            getFile(fileServer[1], key, {}, {}, showProgress, port[1], {});
#endif
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
          bar.tick();
        } else {
          if (value == sha256_file((char*)(key.c_str()), {})) {
            std::ofstream localFilelist("localFilelist.json");
            localFilelistJson["files"][key] = value;
            localFilelist << localFilelistJson;
            localFilelist.close();
          } else if (localFilelistJson["files"][key].is_null() ||
                     localFilelistJson["files"][key] != value) {
            // File not found in local filelist
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || \
    defined(__CYGWIN__) || defined(_WIN64)
            getFile(fileServer[0], key, {}, {}, showProgress, port[0],
                    progressCallback);
#else
            getFile(fileServer[1], key, {}, {}, showProgress, port[1],
                    progressCallback);
#endif
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
        }
      }
    }

    if (!skipVerify) {
      // Verify the two filelists
      for (auto& [key, value] : filelistJson["files"].items()) {
        if (showProgress && progressCallback == nullptr) {
          indicators::ProgressBar bar2{
              indicators::option::BarWidth{30},
              indicators::option::Start{"["},
              indicators::option::Fill{"="},
              indicators::option::Lead{">"},
              indicators::option::End{"]"},
              indicators::option::ShowPercentage{true},
              indicators::option::PostfixText{" | Verifying"},
              indicators::option::MaxProgress{filelistJson["files"].size()}};
          if (sha256_file((char*)(key.c_str()), {}) != value) {
            // File not found in local filelist
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || \
    defined(__CYGWIN__) || defined(_WIN64)
            getFile(fileServer[0], key, {}, {}, showProgress, port[0], {});
#else
            getFile(fileServer[1], key, {}, {}, showProgress, port[1], {});
#endif
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
          bar2.tick();
        } else {
          if (sha256_file((char*)(key.c_str()), {}) != value) {
            // File not found in local filelist
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || \
    defined(__CYGWIN__) || defined(_WIN64)
            getFile(fileServer[0], key, {}, {}, showProgress, port[0], {});
#else
            getFile(fileServer[1], key, {}, {}, showProgress, port[1], {});
#endif
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
        }
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
    if (std::filesystem::exists("filelist.json")) std::remove("filelist.json");
    indicators::show_console_cursor(true);
  }
}

void vupdate::setServer(std::string _server) {
  fileServer = {_server, _server};
}

void vupdate::setServer(std::string _win_server, std::string _unix_server) {
  fileServer = {_win_server, _unix_server};
}

void vupdate::setPort(unsigned int _port) { port = {_port, _port}; }

void vupdate::setPort(unsigned int _win_port, unsigned int _unix_port) {
  port = {_win_port, _unix_port};
}

void vupdate::setFiller(std::string _filler) { spaceFiller = _filler; }

void vupdate::setProgress(bool _progress) { showProgress = _progress; }

void vupdate::setSkipVerify(bool _verify) { skipVerify = _verify; }

void vupdate::setJson(bool _json) { readFromJson = _json; }

void vupdate::setCallback(int (*_callback)(void*, curl_off_t, curl_off_t,
                                           curl_off_t, curl_off_t)) {
  if (_callback == nullptr)
    throw std::runtime_error("Found nullptr instead of a callback function");
  progressCallback = _callback;
}
