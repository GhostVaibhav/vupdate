#pragma once
#include <fstream>
#include <iostream>
#include <filesystem>

#include "spdlog/spdlog.h"
#include "openssl/sha.h"
#include "nlohmann/json.hpp"

void sha256_hash_string(unsigned char[SHA256_DIGEST_LENGTH], char[65]);
std::string sha256_file(char*);
void consolid(std::string, std::shared_ptr<spdlog::logger>&, nlohmann::json&);
