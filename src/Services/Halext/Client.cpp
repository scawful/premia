#include "Client.hpp"

#include <curl/curl.h>

#include <string>
namespace premia {
using namespace halext;

static size_t json_write_callback(const char *contents, size_t size,
                                  size_t nmemb, String *s) {
  size_t new_length = size * nmemb;
  try {
    s->append(contents, new_length);
  } catch (const std::bad_alloc &e) {
    std::cout << e.what() << std::endl;
    return 0;
  }
  return new_length;
}

String Client::send_request(CRString endpoint) const {
  CURL *curl;
  CURLcode res;
  String response;

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  res = curl_easy_perform(curl);

  if (res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));

  /* always cleanup */
  curl_easy_cleanup(curl);

  return response;
}
}  // namespace premia