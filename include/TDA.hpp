#ifndef TDA_hpp
#define TDA_hpp

#include "Premia.hpp"
#include "TDAmeritrade/Client.hpp"
#include "TDAmeritrade/Parser.hpp"
#include "TDAmeritrade/Socket.hpp"
#include "TDAmeritrade/Data/Watchlist.hpp"
#include "TDAmeritrade/Data/PricingStructures.hpp"
#include "TDAmeritrade/Data/Account.hpp"
#include "TDAmeritrade/Data/Quote.hpp"
#include "TDAmeritrade/Data/Order.hpp"
#include "TDAmeritrade/Data/PriceHistory.hpp"
#include "TDAmeritrade/Data/OptionChain.hpp"

namespace tda
{
    namespace JSONObject = boost::property_tree;

    class TDAmeritrade
    {
    private:
        std::string accessToken;
        std::string refreshToken;
        std::string consumerKey;

        Premia::ConsoleLogger consoleLogger;
        Account account;
        Client client;
        Parser parser;

    public:

        std::vector<Watchlist> getWatchlistsByAccount(String account_num) const;
        Quote getQuote(String symbol) const;
        Account getAccount(String account_id);
        PriceHistory getPriceHistory(String ticker, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext) const;
        OptionChain getOptionChain(String ticker, String contractType, String strikeCount,
                                   bool includeQuotes, String strategy, String range,
                                   String expMonth, String optionType) const;

        void postOrder(String account_id, const Order & order) const;

        void startStreamingSession();
        void fetchAccessToken();
        bool is_session_active();

        void addLogger(const Premia::ConsoleLogger &);
        void addAuth(const std::string, const std::string);
    };

}

#endif /* TDAmeritrade_hpp */