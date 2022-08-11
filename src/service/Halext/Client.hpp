#ifndef HalextClient_hpp
#define HalextClient_hpp

#include <curl/curl.h>

#include <iostream>
#include <string>

namespace premia {
namespace halext {
class Client {
 private:
  std::string username;

 public:
  Client() = default;

  std::string send_request(const std::string &endpoint) const;
};
}  // namespace halext
}  // namespace premia
#endif