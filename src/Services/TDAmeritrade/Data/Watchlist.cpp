#include "Watchlist.hpp"

tda::Watchlist::Watchlist()=default;

int tda::Watchlist::getNumInstruments() const
{
    return instruments.size();
}

std::string tda::Watchlist::getName() const
{
    return name;
}

std::string tda::Watchlist::getInstrumentSymbol( int item_id )
{
    return instruments[item_id].getSymbol();
}

std::string tda::Watchlist::getInstrumentDescription( int item_id )
{
    return instruments[item_id].getDescription();
}

std::string tda::Watchlist::getInstrumentType( int item_id )
{
    return instruments[item_id].getType();
}

void tda::Watchlist::setId( int id )
{
    this->watchlist_id = id;
}

void tda::Watchlist::setName(const std::string & name)
{
    this->name = name;
}

void tda::Watchlist::addInstrument(const std::string & symbol, const std::string & desc, const std::string &type)
{
    instruments.push_back(WatchlistInstrument(symbol, desc, type));
}

void tda::Watchlist::addVariable(const std::string & key, const std::string & value)
{
    variables[key] = value;
}
