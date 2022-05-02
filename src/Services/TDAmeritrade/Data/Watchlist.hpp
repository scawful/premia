#ifndef Watchlist_hpp
#define Watchlist_hpp

#include "Premia.hpp"

namespace tda 
{
    class Watchlist 
    {
    public:
        struct WatchlistInstrument {
            String symbol;
            String description;
            String asset_type;

            WatchlistInstrument(CRString sym, CRString desc, CRString type) : symbol(sym), description(desc), asset_type(type) { }

            String getSymbol() const {
                return this->symbol;
            }

            String getDescription() const {
                return this->description;
            }

            String getType() const {
                return this->asset_type;
            }
        };

    private:
        std::vector<WatchlistInstrument> instruments;
        StringMap variables;

        // "name": "string",
        // "watchlistId": "string",
        // "accountId": "string",
        // "status": "'UNCHANGED' or 'CREATED' or 'UPDATED' or 'DELETED'",
        String name;
        unsigned int watchlist_id;
        String account_id;

    public:
        Watchlist();

        int getNumInstruments() const;

        String getName() const;
        String getInstrumentSymbol(int item_id);
        String getInstrumentDescription(int item_id);
        String getInstrumentType(int item_id);

        void setId( int id );
        void setName(CRString name);
        void setAccountId(CRString accountId);
        void setWatchlistVariable(CRString variable);

        void addVariable(CRString key, CRString value);
        void addInstrument(CRString symbol, CRString desc, CRString type);
    };
}

#endif