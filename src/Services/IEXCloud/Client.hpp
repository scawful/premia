#ifndef IEXClient_hpp
#define IEXClient_hpp

#include "Library/Boost.hpp"
#include "Library/Curl.hpp"
#include "Premia.hpp"

namespace Premia {
namespace iex {
class Client {
 private:
  bool sandbox_mode;
  String api_key;
  String base_endpoint;
  String sandbox_endpoint;
  String token_parameter;
  String current_endpoint();

 public:
  Client();

  String send_request(String endpoint);
  String send_authorized_request(String endpoint);

  String get_fund_ownership(String symbol);
  String get_insider_transactions(String symbol);
};
}  // namespace iex
}  // namespace Premia
#endif