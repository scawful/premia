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

#include "Metatypes.hpp"

namespace premia {
namespace cbp {
class Client {
 private:
  bool sandbox = true;
  String api_key;
  String secret_key;
  String passphrase;
  String endpoint_url;

  unsigned int hash_length;
  std::array<unsigned char, EVP_MAX_MD_SIZE> hash;

  static size_t json_write_callback(const char* contents, size_t size,
                                    size_t nmemb, String* s);
  String get_server_time() const;

 public:
  Client();
  ~Client();

  String send_request(CRString request);
  void send_request(String request, String method, String body);
};
}  // namespace cbp
}  // namespace premia
#endif