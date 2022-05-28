#include "Client.hpp"
namespace Premia {
namespace cbp {

/**
 * @brief Modified based on
 *        https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl
 *
 * @param to_encode
 * @return String
 */
auto EncodeBase64(std::string_view to_encode) -> String {
  /// @sa https://www.openssl.org/docs/manmaster/man3/EVP_EncodeBlock.html

  const auto predicted_len =
      4 * ((to_encode.length() + 2) / 3);  // predict output size

  const auto output_buffer{std::make_unique<char[]>(predicted_len + 1)};

  const ArrayList<unsigned char> vec_chars{
      to_encode.begin(),
      to_encode.end()};  // convert to_encode into uchar container

  const auto output_len =
      EVP_EncodeBlock(reinterpret_cast<unsigned char *>(output_buffer.get()),
                      vec_chars.data(), static_cast<int>(vec_chars.size()));

  if (predicted_len != static_cast<unsigned long>(output_len)) {
    throw std::invalid_argument(
        "EncodeBase64 error: predicted length != output length ");
  }

  return output_buffer.get();
}

/**
 * @brief Modified based on
 *        https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl
 *
 * @param to_decode
 * @return String
 */
auto DecodeBase64(std::string_view to_decode) -> String {
  /// @sa https://www.openssl.org/docs/manmaster/man3/EVP_DecodeBlock.html

  const auto predicted_len = 3 * to_decode.length() / 4;  // predict output size

  const auto output_buffer{std::make_unique<char[]>(predicted_len + 1)};

  const ArrayList<unsigned char> vec_chars{
      to_decode.begin(),
      to_decode.end()};  // convert to_decode into uchar container

  const auto output_len =
      EVP_DecodeBlock(reinterpret_cast<unsigned char *>(output_buffer.get()),
                      vec_chars.data(), static_cast<int>(vec_chars.size()));

  if (predicted_len != static_cast<unsigned long>(output_len)) {
    throw std::invalid_argument(
        "DecodeBase64 error: predicted length != output length");
  }

  return output_buffer.get();
}

/**
 * @brief Get JSON data from server and store into CRString
 * @author @scawful
 *
 * @param contents
 * @param size
 * @param nmemb
 * @param s
 * @return size_t
 */
size_t Client::json_write_callback(const char *contents, size_t size,
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

/**
 * @brief Get the server time from Coinbase
 * @author @scawful
 *
 * @return String
 */
String Client::get_server_time() const {
  CURL *curl;
  String response;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();

  // create header structure
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // enables TLSv1.2 / TLSv1.3 version only
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

  // specify the user agent
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

  // specify the request (server time)
  String request_path = endpoint_url + "/time";
  curl_easy_setopt(curl, CURLOPT_URL, request_path.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  // perform the request and clean it up (duh!)
  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  // this can be decomposed into a Parser class
  std::istringstream json_response(response);
  boost::property_tree::ptree property_tree;

  try {
    read_json(json_response, property_tree);
  } catch (const boost::property_tree::ptree_error &e) {
    std::cout << e.what() << std::endl;
  }

  String server_time;
  try {
    server_time = property_tree.get_child("epoch.").get_value<String>();
  } catch (const boost::property_tree::ptree_error &e) {
    std::cout << e.what() << std::endl;
  }
  server_time = server_time.substr(0, server_time.find('.'));

  return server_time;
}

/**
 * @brief Construct a new Client:: Client object
 * @todo make the entire namespace and object structure into "Coinbase" and just
 * add a flag for Coinbase Pro, since the APIs are identical basically
 *
 * CRString api, CRString secret, CRString passphrase, bool sandbox
 * : api_key(api), secret_key(secret), passphrase(passphrase)
 *
 * @param api
 * @param secret
 * @param passphrase
 * @param sandbox
 */
Client::Client() {
  endpoint_url = sandbox ? "https://api.pro.coinbase.com"
                         : "https://api-public.sandbox.pro.coinbase.com";
}

Client::~Client() = default;

/**
 * @brief Coinbase API Header Signature Encoding Request Routine
 * @author @scawful
 *
 * @param filename
 * @param request
 */
String Client::send_request(CRString request) {
  CURL *curl;
  String response;

  // initialize libcurl
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();

  // enables TLSv1.2 / TLSv1.3 version only
  curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

  // verbose reports more detailed errors, can be removed if you want
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

  // create header structure
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // set the access key
  String access_key = "CB-ACCESS-KEY: " + api_key;
  headers = curl_slist_append(headers, access_key.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // prepare the message contents to be encoded
  String _body = "";
  String _method = "GET";
  String _request_path = endpoint_url + request.data();
  String _timestamp = get_server_time();
  String _message = _timestamp + _method + request.data() + _body;

  std::cout << _message << std::endl;

  // base64 decode the secret key
  String decoded_key = DecodeBase64(secret_key.data());
  char *hmac_key = strdup(decoded_key.c_str());
  size_t key_len = strlen(hmac_key);

  const auto *data = (const unsigned char *)strdup(_message.c_str());
  size_t data_len = strlen((char *)_message.c_str());

  unsigned char *md = nullptr;
  unsigned int md_len = -1;

  // HMAC sha256
  md = HMAC(EVP_sha256(), hmac_key, static_cast<int>(key_len), data,
            static_cast<int>(data_len), md, &md_len);

  // base64 encode the hmac signature data
  const char *pre_encode_signature_c =
      strdup(reinterpret_cast<const char *>(md));
  String pre_encode_signature(pre_encode_signature_c);
  String post_encode_signature = EncodeBase64(pre_encode_signature);

  // free data related to encoding signature
  free(hmac_key);
  free((char *)data);
  free((char *)pre_encode_signature_c);

  // add the signature to the header
  String access_sign = "CB-ACCESS-SIGN: " + post_encode_signature;
  headers = curl_slist_append(headers, access_sign.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // add the timestamp to the header
  String timestamp = "CB-ACCESS-TIMESTAMP: " + _timestamp;
  headers = curl_slist_append(headers, timestamp.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // add the passphrase to the header
  String _passphrase = passphrase;
  String request_passphrase = "CB-ACCESS-PASSPHRASE: " + _passphrase;
  headers = curl_slist_append(headers, request_passphrase.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // specify the user agent
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "CoinbaseProAPI");

  curl_easy_setopt(curl, CURLOPT_URL, _request_path.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  return response;
}

// void Client::send_request( String filename, String request, String method,
// String body )
// {

// }
}  // namespace cbp
}  // namespace Premia