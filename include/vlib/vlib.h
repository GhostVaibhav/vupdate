#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <string>

#include "curl/curl.h"

class vupdate {
 private:
  std::vector<std::string> fileServer;
  std::vector<unsigned int> port;
  std::string spaceFiller;
  bool showProgress;
  bool skipVerify;
  bool readFromJson;
  int (*progressCallback)(void*, curl_off_t, curl_off_t, curl_off_t,
                          curl_off_t);

 public:
  vupdate()
      : fileServer({"", ""}),
        port({0, 0}),
        spaceFiller("+"),
        showProgress(true),
        readFromJson(false),
        skipVerify(false),
        progressCallback(nullptr) {}

  vupdate(std::string _server)
      : fileServer({_server, _server}),
        port({0, 0}),
        spaceFiller("+"),
        showProgress(true),
        readFromJson(false),
        skipVerify(false),
        progressCallback(nullptr) {}

  vupdate(std::string _server, unsigned int _port)
      : fileServer({_server, _server}),
        port({_port, _port}),
        spaceFiller("+"),
        showProgress(true),
        readFromJson(false),
        skipVerify(false),
        progressCallback(nullptr) {}

  vupdate(std::string _win_server, std::string _unix_server)
      : fileServer({_win_server, _unix_server}),
        port({0, 0}),
        spaceFiller("+"),
        showProgress(true),
        readFromJson(false),
        skipVerify(false),
        progressCallback(nullptr) {}

  vupdate(std::string _win_server, unsigned int _win_port,
          std::string _unix_server, unsigned int _unix_port)
      : fileServer({_win_server, _unix_server}),
        port({_win_port, _unix_port}),
        spaceFiller("+"),
        showProgress(true),
        readFromJson(false),
        skipVerify(false),
        progressCallback(nullptr) {}

  void update();

  void setServer(std::string _server);

  void setServer(std::string _win_server, std::string _unix_server);

  void setPort(unsigned int _port);

  void setPort(unsigned int _win_port, unsigned int _unix_port);

  void setFiller(std::string _filler);

  void setProgress(bool _process);

  void setSkipVerify(bool _verify);

  void setJson(bool _json);

  void setCallback(int (*)(void*, curl_off_t, curl_off_t, curl_off_t,
                           curl_off_t));
};
