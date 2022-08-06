#ifndef IEXClient_hpp
#define IEXClient_hpp

#include <curl/curl.h>

#include <string>

#include "metatypes.h"
#include "premia.h"


namespace premia {
namespace iex {
class Client {
 private:
  bool sandbox_mode;
  std::string api_key;
  std::string base_endpoint;
  std::string sandbox_endpoint;
  std::string token_parameter;
  std::string current_endpoint();

 public:
  Client();

  std::string send_request(std::string endpoint);
  std::string send_authorized_request(std::string endpoint);

  std::string get_fund_ownership(std::string symbol);
  std::string get_insider_transactions(std::string symbol);
};
}  // namespace iex
}  // namespace premia
#endif