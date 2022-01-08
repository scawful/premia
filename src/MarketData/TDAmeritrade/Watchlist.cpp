#include "Watchlist.hpp"

tda::Watchlist::Watchlist() 
{

}

int tda::Watchlist::getNumInstruments()
{
    return instruments.size();
}

std::string tda::Watchlist::getName()
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

void tda::Watchlist::setName( std::string name )
{
    this->name = name;
}

void tda::Watchlist::addInstrument(std::string symbol, std::string desc, std::string type)
{
    instruments.push_back(WatchlistInstrument(symbol, desc, type));
}

void tda::Watchlist::addVariable(std::string key, std::string value)
{
    variables[key] = value;
}
