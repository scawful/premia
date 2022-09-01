#ifndef TDA_hpp
#define TDA_hpp

#include "service/TDAmeritrade/client.h"
#include "service/TDAmeritrade/parser.h"
#include "service/TDAmeritrade/socket.h"
#include "service/TDAmeritrade/data/Account.hpp"
#include "service/TDAmeritrade/data/OptionChain.hpp"
#include "service/TDAmeritrade/data/Order.hpp"
#include "service/TDAmeritrade/data/PriceHistory.hpp"
#include "service/TDAmeritrade/data/PricingStructures.hpp"
#include "service/TDAmeritrade/data/Quote.hpp"
#include "service/TDAmeritrade/data/Watchlist.hpp"

namespace premia::tda {
using Watchlists = std::vector<Watchlist>;

class TDA {
 private:
  TDA() = default;
  bool auth = false;
  Account account;
  Client client;
  Parser parser;

 public:
  TDA(TDA const &) = delete;
  void operator=(TDA const &) = delete;
  static TDA &getInstance() {
    static TDA instance;
    return instance;
  }

  auto authUser(const std::string &key, const std::string &token) -> void {
    if (!key.empty() && !token.empty()) {
      client.addAuth(key, token);
      auth = true;
    }
  }

  auto isAuth() const -> bool { return auth; }

  auto getQuote(const std::string &symbol) const -> Quote {
    std::string response = client.get_quote(symbol);
    return parser.parse_quote(parser.read_response(response));
  }

  auto getAccount(const std::string &accountNumber) -> Account {
    std::string response = client.get_account(accountNumber);
    return parser.parse_account(parser.read_response(response));
  }

  auto getAllAccounts() -> Account {
    std::string response = client.get_all_accounts();
    return parser.parse_all_accounts(parser.read_response(response));
  }

  auto getPriceHistory(const std::string &ticker, PeriodType periodType,
                       FrequencyType frequencyType, int periodAmount,
                       int frequencyAmount, bool extendedHoursTrading) const
      -> PriceHistory {
    std::string response = client.get_price_history(
        ticker, periodType, periodAmount, frequencyType, frequencyAmount,
        extendedHoursTrading);
        std::cout << response << std::endl;
    return parser.parse_price_history(parser.read_response(response), ticker,
                                      frequencyType);
  }

  auto getOptionChain(const std::string &ticker, const std::string &strikeCount,
                      const std::string &strategy, const std::string &range,
                      const std::string &expMonth,
                      const std::string &optionType) const -> OptionChain {
    std::string response =
        client.get_option_chain(ticker, "ALL", strikeCount, true, strategy,
                                range, expMonth, optionType);
    return parser.parse_option_chain(parser.read_response(response));
  }

  auto getWatchlistsByAccount(const std::string &account_num) const
      -> Watchlists {
    std::string response = client.get_watchlist_by_account(account_num);
    return parser.parse_watchlist_data(parser.read_response(response));
  }

  auto getAllAcountNumbers() -> std::vector<std::string> {
    auto list = client.get_all_account_ids();
    return list;
  }

  auto getDefaultAccount() -> std::string {
    auto list = getAllAcountNumbers();
    auto num = list.at(0);
    return num;
  }

  auto parseOptionSymbol(const std::string &symbol) const -> std::string {
    return parser.parse_option_symbol(symbol);
  }

  void sendChartRequestToSocket(const std::string &ticker) {
    client.start_session(ticker);
  }

  void sendSocketLogout() { client.send_logout_request(); }
};

}  // namespace premia::tda

#endif