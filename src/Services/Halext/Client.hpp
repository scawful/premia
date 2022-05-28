#ifndef HalextClient_hpp
#define HalextClient_hpp

#include "Library/Curl.hpp"
#include "Metatypes.hpp"
#include "Premia.hpp"

namespace Premia {
namespace halext {
class Client {
 private:
  String username;

 public:
  Client() = default;

  String send_request(CRString endpoint) const;
};
}  // namespace halext
}  // namespace Premia
#endif