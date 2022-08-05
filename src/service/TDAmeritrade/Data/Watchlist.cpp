#include "Watchlist.hpp"
namespace premia {
tda::Watchlist::Watchlist() = default;

int tda::Watchlist::getNumInstruments() const {
  return ((int) instruments.size());
}

String tda::Watchlist::getName() const { return name; }

String tda::Watchlist::getInstrumentSymbol(int item_id) {
  return instruments[item_id].getSymbol();
}

String tda::Watchlist::getInstrumentDescription(int item_id) {
  return instruments[item_id].getDescription();
}

String tda::Watchlist::getInstrumentType(int item_id) {
  return instruments[item_id].getType();
}

void tda::Watchlist::setId(int id) { this->watchlist_id = id; }

void tda::Watchlist::setName(CRString newName) { this->name = newName; }

void tda::Watchlist::setAccountId(CRString accountId) {
  this->account_id = accountId;
}

void tda::Watchlist::addInstrument(CRString symbol, CRString desc,
                                   CRString type) {
  instruments.emplace_back(symbol, desc, type);
}

void tda::Watchlist::removeInstrument(int idx) {
  ArrayList<WatchlistInstrument>::iterator iidx = instruments.begin();
  advance(iidx, idx);
  instruments.erase(iidx);
}

void tda::Watchlist::addVariable(CRString key, CRString value) {
  variables[key] = value;
}

bool tda::Watchlist::containsTicker(String ticker) {
  // Sheesh C++ is wierd with its lambdas...
  return std::find_if(instruments.begin(),
                      instruments.end(),
                      [&s = ticker](const WatchlistInstrument &wli) -> bool { return s == wli.getSymbol(); })
      != instruments.end();
}

}  // namespace premia
