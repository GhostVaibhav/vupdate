#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <string>

#include "curl/curl.h"

class vupdate {
 private:
  std::string fileServer;
  unsigned int port;
  std::string spaceFiller;
  bool showProgress;
  bool skipVerify;
  bool readFromJson;
  int (*progressCallback)(void*, curl_off_t, curl_off_t, curl_off_t,
                          curl_off_t);

 public:
  vupdate()
      : fileServer(""),
        spaceFiller("+"),
        showProgress(true),
        skipVerify(false),
        readFromJson(true),
        port(0),
        progressCallback(nullptr) {}

  vupdate(std::string _server)
      : fileServer(_server),
        spaceFiller("+"),
        showProgress(true),
        readFromJson(false),
        skipVerify(false),
        port(0),
        progressCallback(nullptr) {}

  vupdate(std::string _server, unsigned int _port)
      : fileServer(_server),
        port(_port),
        spaceFiller("+"),
        showProgress(true),
        readFromJson(false),
        skipVerify(false),
        progressCallback(nullptr) {}

  void update();

  void setServer(std::string _server);

  void setPort(unsigned int _port);

  void setFiller(std::string _filler);

  void setProgress(bool _process);

  void setSkipVerify(bool _verify);

  void setJson(bool _json);

  void setCallback(int (*)(void*, curl_off_t, curl_off_t, curl_off_t,
                           curl_off_t));
};
