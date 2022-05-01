#include "Watchlist.hpp"

tda::Watchlist::Watchlist()=default;

int tda::Watchlist::getNumInstruments() const
{
    return ((int) instruments.size());
}

std::string tda::Watchlist::getName() const
{
    return name;
}

std::string tda::Watchlist::getInstrumentSymbol(int item_id)
{
    return instruments[item_id].getSymbol();
}

std::string tda::Watchlist::getInstrumentDescription(int item_id)
{
    return instruments[item_id].getDescription();
}

std::string tda::Watchlist::getInstrumentType(int item_id)
{
    return instruments[item_id].getType();
}

void tda::Watchlist::setId(int id)
{
    this->watchlist_id = id;
}

void tda::Watchlist::setName(String newName)
{
    this->name = newName;
}

void tda::Watchlist::setAccountId(String accountId)
{
    this->account_id = accountId;
}

void tda::Watchlist::addInstrument(String symbol, String desc, String type)
{
    instruments.emplace_back(symbol, desc, type);
}

void tda::Watchlist::addVariable(String key, String value)
{
    variables[key] = value;
}
