#define _CRT_SECURE_NO_WARNINGS

#include "vconsolid/vconsolid.h"

void sha256_hash_string(unsigned char hash[SHA256_DIGEST_LENGTH],
                        char outputBuffer[65]) {
  int i = 0;

  for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
  }

  outputBuffer[64] = 0;
}

std::string sha256_file(char *path) {
  char outputBuffer[65];
  FILE *file = fopen(path, "rb");
  if (!file) return "";

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  const int bufSize = 32768;
  char *buffer = (char *)malloc(bufSize);
  size_t bytesRead = 0;
  if (!buffer) return "";
  while ((bytesRead = fread(buffer, 1, bufSize, file))) {
    SHA256_Update(&sha256, buffer, bytesRead);
  }
  SHA256_Final(hash, &sha256);

  sha256_hash_string(hash, outputBuffer);
  fclose(file);
  free(buffer);
  return std::string(outputBuffer);
}

void consolid(std::string file, std::shared_ptr<spdlog::logger> &logger,
              nlohmann::json &localFilelistJson) {
  logger->info("Consolidating: {}", file);
  std::string directories = "", rawFile = "";
  size_t lastIndex = file.find_last_of('/');
  if (lastIndex < file.size()) {
    rawFile = file.substr(lastIndex + 1);
  } else {
    rawFile = file;
  }
  logger->info("Raw file: {}", rawFile);
  if (lastIndex < file.size()) directories.append(file.substr(0, lastIndex));
  logger->info("Directories: {}", directories);
  if (lastIndex < file.size()) {
    directories = file.substr(0, lastIndex);
    if (!std::filesystem::exists(directories))
      if (std::filesystem::create_directories(directories))
        logger->info("Created directory: {}", directories);
    if (lastIndex != 1) {
      std::ofstream localFilelist("localFilelist.json");
      localFilelistJson["files"][file] = sha256_file((char *)rawFile.c_str());
      std::filesystem::copy_file(
          "./" + rawFile, directories + "/" + rawFile,
          std::filesystem::copy_options::overwrite_existing);
      std::remove(rawFile.c_str());
      localFilelist << localFilelistJson;
      localFilelist.close();
    }
  }
}
