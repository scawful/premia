#include "PriceHistory.hpp"
namespace premia {
using namespace tda;

PriceHistory::PriceHistory() {
  this->tickerSymbol = "";
  this->initialized = false;
  for (int i = 0; i < 4; i++) {
    std::vector<tda::Candle> vector;
    candles.push_back(vector);
  }
}

void PriceHistory::addCandle(tda::Candle candle) {
  candleVector.push_back(candle);
}

void PriceHistory::addCandleByType(Candle candle, int type) {
  candles[type].push_back(candle);
}

std::vector<tda::Candle> PriceHistory::getCandles(int index) {
  return candles[index];
}

int PriceHistory::getNumCandles(int index) { return candles[index].size(); }

std::vector<tda::Candle> PriceHistory::getCandleVector() { return candleVector; }

std::string PriceHistory::getCandleDataVariable(std::string variable) {
  return candleData[variable];
}

std::string PriceHistory::getPriceHistoryVariable(std::string variable) {
  return priceHistoryVariables[variable];
}

std::string PriceHistory::getTickerSymbol() { return tickerSymbol; }

bool PriceHistory::getInitialized() { return initialized; }

void PriceHistory::setPriceHistoryVariable(std::string key, std::string value) {
  priceHistoryVariables[key] = value;
}

void PriceHistory::setTickerSymbol(std::string ticker) {
  this->tickerSymbol = ticker;
}

void PriceHistory::setInitialized() { this->initialized = true; }

void PriceHistory::clear() {
  priceHistoryVariables.clear();
  candleVector.clear();
}
}  // namespace premia