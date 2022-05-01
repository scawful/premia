#ifndef Watchlist_hpp
#define Watchlist_hpp

#include "Premia.hpp"

namespace tda 
{
    class Watchlist 
    {
    public:
        struct WatchlistInstrument {
            std::string symbol;
            std::string description;
            std::string asset_type;

            WatchlistInstrument(String sym, String desc, String type) : symbol(sym), description(desc), asset_type(type) { }

            std::string getSymbol() const {
                return this->symbol;
            }

            std::string getDescription() const {
                return this->description;
            }

            std::string getType() const {
                return this->asset_type;
            }
        };

    private:
        std::vector<WatchlistInstrument> instruments;
        std::unordered_map<std::string, std::string> variables;

        // "name": "string",
        // "watchlistId": "string",
        // "accountId": "string",
        // "status": "'UNCHANGED' or 'CREATED' or 'UPDATED' or 'DELETED'",
        std::string name;
        unsigned int watchlist_id;
        std::string account_id;

    public:
        Watchlist();

        int getNumInstruments() const;

        std::string getName() const;
        std::string getInstrumentSymbol(int item_id);
        std::string getInstrumentDescription(int item_id);
        std::string getInstrumentType(int item_id);

        void setId( int id );
        void setName(String name);
        void setAccountId(String accountId);
        void setWatchlistVariable(String variable);

        void addVariable(String key, String value);
        void addInstrument(String symbol, String desc, String type);
    };
}

#endif