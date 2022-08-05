#include "Client.hpp"

#include <curl/curl.h>

#include <string>

#include "Metatypes.hpp"
#include "Premia.hpp"

namespace premia {
namespace iex {

static size_t json_write(const char *contents, size_t size, size_t nmemb,
                         std::string *s) {
  size_t new_length = size * nmemb;
  try {
    s->append(contents, new_length);
  } catch (const std::bad_alloc &e) {
    // @todo attach a logger
    return EXIT_FAILURE;
  }
  return new_length;
}

String Client::current_endpoint() {
  if (sandbox_mode)
    return this->sandbox_endpoint;
  else
    return this->base_endpoint;
}

Client::Client() {
  this->base_endpoint = "https://cloud.iexapis.com/";
  this->sandbox_endpoint = "https://sandbox.iexapis.com/stable/";
  this->token_parameter = "?token=" + api_key;
  this->sandbox_mode = true;
}

/**
 * @brief
 *
 * @param endpoint
 * @return String
 */
String Client::send_request(String endpoint) {
  CURL *curl;
  CURLcode res;
  String response;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) throw premia::ClientException();

    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  return response;
}

/**
 * @brief
 *
 * @param endpoint
 * @return String
 */
String Client::send_authorized_request(String endpoint) {
  CURL *curl;
  CURLcode res;
  String response;

  curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers = NULL;
    String auth_bearer = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_bearer.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) throw premia::ClientException();

    curl_easy_cleanup(curl);
  }

  return response;
}

String Client::get_fund_ownership(String symbol) {
  String endpoint = current_endpoint() + "stock/" + symbol +
                    "/fund-ownership/" + token_parameter;
  return send_request(endpoint);
}

String Client::get_insider_transactions(String symbol) {
  String endpoint = current_endpoint() + "stock/" + symbol +
                    "/insider-transactions/" + token_parameter;
  return send_request(endpoint);
}
}  // namespace iex
}  // namespace premia