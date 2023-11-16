#pragma once

#include <math.h>

#include <filesystem>
#include <iostream>
#include <optional>

#include "curl/curl.h"
#include "indicators/indicators.hpp"
#include "spdlog/spdlog.h"

size_t write_data(void*, size_t, size_t, FILE*);
int download_progress_callback(void*, curl_off_t, curl_off_t, curl_off_t,
                               curl_off_t);
void getFile(std::string, std::string,
             const std::optional<std::shared_ptr<spdlog::logger>>&,
             const std::optional<std::string>&, const std::optional<bool>&,
             const unsigned int);
