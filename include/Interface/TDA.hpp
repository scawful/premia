#ifndef TDA_hpp
#define TDA_hpp

#include "../src/Services/TDAmeritrade/Client.hpp"
#include "../src/Services/TDAmeritrade/Parser.hpp"
#include "../src/Services/TDAmeritrade/Socket.hpp"
#include "../src/Services/TDAmeritrade/Data/Watchlist.hpp"
#include "../src/Services/TDAmeritrade/Data/PricingStructures.hpp"
#include "../src/Services/TDAmeritrade/Data/Account.hpp"
#include "../src/Services/TDAmeritrade/Data/Quote.hpp"
#include "../src/Services/TDAmeritrade/Data/Order.hpp"
#include "../src/Services/TDAmeritrade/Data/PriceHistory.hpp"
#include "../src/Services/TDAmeritrade/Data/OptionChain.hpp"

namespace tda
{
    using Watchlists = std::vector<Watchlist>;

    class TDA 
    {
    private:
        TDA() = default;
        bool auth = false;
        Account account;
        Client client;
        Parser parser;

    public:
        TDA(TDA const&)             = delete;
        void operator=(TDA const&)  = delete;
        static TDA& getInstance() {
            static TDA instance;    
            return instance;
        }

        auto authUser(String key, String token) 
            -> void {
            if (!key.empty() && !token.empty()) {
                client.addAuth(key, token);
                auth = true;
            }
        }

        auto isAuth() const
            -> bool {
            return auth;
        }

        auto getQuote(String symbol) 
            -> Quote {
            String response = client.get_quote(symbol);
            return parser.parse_quote(parser.read_response(response)); 
        }

        auto getAccount(String accountNumber) 
            -> Account {
            String response = client.get_account(accountNumber);
            return parser.parse_account(parser.read_response(response));
        }

        auto getAllAccounts() 
            -> Account {
            String response = client.get_all_accounts();
            return parser.parse_all_accounts(parser.read_response(response));
        }

        auto getPriceHistory(String ticker, PeriodType periodType, FrequencyType frequencyType,
                             int periodAmount, int frequencyAmount, bool extendedHoursTrading) 
            -> PriceHistory const {
            String response = client.get_price_history(ticker, periodType, periodAmount, frequencyType, frequencyAmount, extendedHoursTrading);
            return parser.parse_price_history(parser.read_response(response), ticker, frequencyType);
        }

        auto getOptionChain(String ticker, String strikeCount, String strategy, 
                            String range, String expMonth, String optionType) 
            -> OptionChain {
            String response = client.get_option_chain(ticker, "ALL", strikeCount, true, strategy, range, expMonth, optionType);
            return parser.parse_option_chain(parser.read_response(response));
        }

        auto getWatchlistsByAccount(String account_num)
            -> Watchlists {
            String response = client.get_watchlist_by_account(account_num);
            return parser.parse_watchlist_data(parser.read_response(response));
        }

        auto getAllAcountNumbers()
            -> StringList {
            auto list = client.get_all_account_ids();
            return list;
        }

        auto getDefaultAccount()
            -> String {
            auto list = getAllAcountNumbers();
            auto num = list.at(0);
            return num;
        }

        auto parseOptionSymbol(CRString symbol) 
            -> String {
            return parser.parse_option_symbol(symbol);
        }

        void postOrder(CRString account_id, const Order & order) {
            client.post_order(account_id, order);
        }

        void startSession() {
            client.start_session();
        }

        void sendSocketLogin() {
            client.send_login_request();
        }

        void sendChartRequestToSocket(String ticker) {
            client.send_basic_quote_request(ticker);
        }

        void sendSocketLogout() {
            client.send_logout_request();
        }
    };

}

#endif /* TDAmeritrade_hpp */