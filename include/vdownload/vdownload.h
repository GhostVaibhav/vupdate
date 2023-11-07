#pragma once
#include <iostream>
#include <math.h>
#include <filesystem>

#include "curl/curl.h"
#include "spdlog/spdlog.h"
#include "single_include/indicators/indicators.hpp"

size_t write_data(void*, size_t, size_t, FILE*);
int progress_func(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t);
void getFile(std::string, std::string, std::shared_ptr<spdlog::logger>&);
