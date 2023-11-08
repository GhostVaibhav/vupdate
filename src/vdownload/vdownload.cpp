#define _CRT_SECURE_NO_WARNINGS

#include "vdownload/vdownload.h"

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
  size_t written = fwrite(ptr, size, nmemb, stream);
  return written;
}

int progress = -1;

int download_progress_callback(void* clientp, curl_off_t dltotal,
                               curl_off_t dlnow, curl_off_t ultotal,
                               curl_off_t ulnow) {
  if ((dltotal == 0 && progress == -1) ||
      (dltotal && progress != static_cast<float>(dlnow) /
                                  static_cast<float>(dltotal) * 100)) {
    indicators::ProgressBar* progress_bar =
        static_cast<indicators::ProgressBar*>(clientp);

    if (progress_bar->is_completed()) {
      return 0;
    } else if (dltotal == 0) {
      progress = 0;
      progress_bar->set_progress(0);
    } else {
      float percentage =
          static_cast<float>(dlnow) / static_cast<float>(dltotal) * 100;
      progress = static_cast<int>(percentage);
      progress_bar->set_progress(static_cast<size_t>(percentage));
    }
  }

  return 0;
}

void getFile(std::string server, std::string file,
             std::shared_ptr<spdlog::logger>& logger) {
  indicators::ProgressBar progress_bar{
      indicators::option::BarWidth{30},
      indicators::option::Start{" ["},
      indicators::option::Fill{"="},
      indicators::option::Lead{">"},
      indicators::option::End{"]"},
      indicators::option::PostfixText{" | " + file.substr(0, 64)},
      indicators::option::ShowPercentage{true},
      indicators::option::MaxPostfixTextLen{15}};

  progress = -1;

  std::string rawFile = "";
  file = file.substr(2);
  size_t pos = file.find_last_of('/');
  if (pos != 1) {
    rawFile = file.substr(pos + 1);
  } else {
    rawFile = file;
  }
  std::replace(file.begin(), file.end(), ' ', '+');
  CURL* curl;
  CURLcode res;
  FILE* fp;
  curl = curl_easy_init();
  if (curl) {
    fp = fopen(rawFile.c_str(), "wb");
    curl_easy_setopt(curl, CURLOPT_URL, (server + "/" + file).c_str());
    logger->info("Downloading: {}", server + "/" + file);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0l);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0l);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0l);
    curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1l);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION,
                     download_progress_callback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA,
                     static_cast<void*>(&progress_bar));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      throw std::exception(curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
    fclose(fp);
  }
}
