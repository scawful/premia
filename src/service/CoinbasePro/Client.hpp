#ifndef CBP_Client_hpp
#define CBP_Client_hpp

#include <curl/curl.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>

#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>

#include "metatypes.h"

namespace premia {
namespace cbp {
class Client {
 private:
  bool sandbox = true;
  std::string api_key;
  std::string secret_key;
  std::string passphrase;
  std::string endpoint_url;

  unsigned int hash_length;
  std::array<unsigned char, EVP_MAX_MD_SIZE> hash;

  static size_t json_write_callback(const char* contents, size_t size,
                                    size_t nmemb, std::string* s);
  std::string get_server_time() const;

 public:
  Client();
  ~Client();

  std::string send_request(const std::string &request);
  void send_request(std::string request, std::string method, std::string body);
};
}  // namespace cbp
}  // namespace premia
#endif