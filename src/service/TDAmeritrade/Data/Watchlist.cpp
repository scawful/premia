#include "Watchlist.hpp"

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace premia {
namespace tda {

Watchlist::Watchlist() = default;

int Watchlist::getNumInstruments() const { return ((int)instruments.size()); }

std::string Watchlist::getName() const { return name; }

std::string Watchlist::getInstrumentSymbol(int item_id) {
  return instruments[item_id].getSymbol();
}

std::string Watchlist::getInstrumentDescription(int item_id) {
  return instruments[item_id].getDescription();
}

std::string Watchlist::getInstrumentType(int item_id) {
  return instruments[item_id].getType();
}

void Watchlist::setId(int id) { this->watchlist_id = id; }

void Watchlist::setName(const std::string &newName) { this->name = newName; }

void Watchlist::setAccountId(const std::string &accountId) {
  this->account_id = accountId;
}

void Watchlist::addInstrument(const std::string &symbol,
                              const std::string &desc,
                              const std::string &type) {
  instruments.emplace_back(symbol, desc, type);
}

void Watchlist::removeInstrument(int idx) {
  std::vector<WatchlistInstrument>::iterator iidx = instruments.begin();
  advance(iidx, idx);
  instruments.erase(iidx);
}

void Watchlist::addVariable(const std::string &key, const std::string &value) {
  variables[key] = value;
}

bool Watchlist::containsTicker(std::string ticker) {
  // Sheesh C++ is wierd with its lambdas...
  return std::find_if(instruments.begin(), instruments.end(),
                      [&s = ticker](const WatchlistInstrument &wli) -> bool {
                        return s == wli.getSymbol();
                      }) != instruments.end();
}

}  // namespace tda
}  // namespace premia
