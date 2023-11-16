#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#include "nlohmann/json.hpp"
#include "openssl/sha.h"
#include "spdlog/spdlog.h"

void sha256_hash_string(unsigned char[SHA256_DIGEST_LENGTH], char[65]);
std::string sha256_file(char*,
                        const std::optional<std::shared_ptr<spdlog::logger>>&);
void consolid(std::string, nlohmann::json&,
              const std::optional<std::shared_ptr<spdlog::logger>>&);
