#ifndef HalextClient_hpp
#define HalextClient_hpp

#include <curl/curl.h>

#include <string>

#include "metatypes.h"
#include "premia.h"

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