#ifndef IEX_hpp
#define IEX_hpp

#include "service/IEXCloud/Client.hpp"
#include "service/IEXCloud/Data/FundOwnership.hpp"
#include "service/IEXCloud/Parser.hpp"

namespace premia {
namespace iex {
class IEX {
 private:
  IEX() {}
  Client client;
  Parser parser;

 public:
  IEX(IEX const&) = delete;
  void operator=(IEX const&) = delete;
  static IEX& getInstance() {
    static IEX instance;
    return instance;
  }

  auto getFundOwnership(String symbol) -> ArrayList<FundOwnership> const {
    String response = client.get_fund_ownership(symbol);
    return parser.parse_fund_ownership(response);
  }
};

}  // namespace iex
}  // namespace premia

#endif