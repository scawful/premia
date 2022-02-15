#ifndef Watchlist_hpp
#define Watchlist_hpp

#include "../../../core.hpp"

namespace tda 
{
    class Watchlist 
    {
    public:
        struct WatchlistInstrument {
            std::string symbol;
            std::string description;
            std::string asset_type;

            WatchlistInstrument(std::string sym, std::string desc, std::string type) {
                this->symbol = sym;
                this->description = desc;
                this->asset_type = type;
            }

            std::string getSymbol() {
                return this->symbol;
            }

            std::string getDescription() {
                return this->description;
            }

            std::string getType() {
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
        unsigned int account_id;

    public:
        Watchlist();

        int getNumInstruments();

        std::string getName();
        std::string getInstrumentSymbol( int item_id );
        std::string getInstrumentDescription( int item_id );
        std::string getInstrumentType( int item_id );

        void setId( int id );
        void setName( std::string name );
        void setWatchlistVariable( std::string variable );

        void addVariable(std::string key, std::string value);
        void addInstrument(std::string symbol, std::string desc, std::string type);
    };
}

#endif