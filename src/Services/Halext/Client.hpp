#ifndef HalextClient_hpp
#define HalextClient_hpp

#include <curl/curl.h>

#include <string>

#include "Metatypes.hpp"
#include "Premia.hpp"

namespace premia {
namespace halext {
class Client {
 private:
  String username;

 public:
  Client() = default;

  String send_request(CRString endpoint) const;
};
}  // namespace halext
}  // namespace premia
#endif