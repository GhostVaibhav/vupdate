#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"
#include "openssl/sha.h"
#include "spdlog/spdlog.h"

void sha256_hash_string(unsigned char[SHA256_DIGEST_LENGTH], char[65]);
std::string sha256_file(char*);
void consolid(std::string, std::shared_ptr<spdlog::logger>&, nlohmann::json&);
