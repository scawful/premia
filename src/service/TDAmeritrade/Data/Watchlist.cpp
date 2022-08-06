#include "Watchlist.hpp"
namespace premia {
tda::Watchlist::Watchlist() = default;

int tda::Watchlist::getNumInstruments() const {
  return ((int) instruments.size());
}

std::string tda::Watchlist::getName() const { return name; }

std::string tda::Watchlist::getInstrumentSymbol(int item_id) {
  return instruments[item_id].getSymbol();
}

std::string tda::Watchlist::getInstrumentDescription(int item_id) {
  return instruments[item_id].getDescription();
}

std::string tda::Watchlist::getInstrumentType(int item_id) {
  return instruments[item_id].getType();
}

void tda::Watchlist::setId(int id) { this->watchlist_id = id; }

void tda::Watchlist::setName(const std::string &newName) { this->name = newName; }

void tda::Watchlist::setAccountId(const std::string &accountId) {
  this->account_id = accountId;
}

void tda::Watchlist::addInstrument(const std::string &symbol, const std::string &desc,
                                   const std::string &type) {
  instruments.emplace_back(symbol, desc, type);
}

void tda::Watchlist::removeInstrument(int idx) {
  std::vector<WatchlistInstrument>::iterator iidx = instruments.begin();
  advance(iidx, idx);
  instruments.erase(iidx);
}

void tda::Watchlist::addVariable(const std::string &key, const std::string &value) {
  variables[key] = value;
}

bool tda::Watchlist::containsTicker(std::string ticker) {
  // Sheesh C++ is wierd with its lambdas...
  return std::find_if(instruments.begin(),
                      instruments.end(),
                      [&s = ticker](const WatchlistInstrument &wli) -> bool { return s == wli.getSymbol(); })
      != instruments.end();
}

}  // namespace premia
