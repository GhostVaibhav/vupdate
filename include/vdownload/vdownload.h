#pragma once
#include <math.h>

#include <filesystem>
#include <iostream>

#include "curl/curl.h"
#include "single_include/indicators/indicators.hpp"
#include "spdlog/spdlog.h"

size_t write_data(void*, size_t, size_t, FILE*);
int download_progress_callback(void*, curl_off_t, curl_off_t, curl_off_t,
                               curl_off_t);
void getFile(std::string, std::string, std::shared_ptr<spdlog::logger>&);
