#ifndef PriceHistory_hpp
#define PriceHistory_hpp

#include "PricingStructures.hpp"
namespace Premia {
namespace tda {
// //CandleList:
// {
// "candles": [
//     {
//     "close": 0,
//     "datetime": 0,
//     "high": 0,
//     "low": 0,
//     "open": 0,
//     "volume": 0
//     }
// ],
// "empty": false,
// "symbol": "string"
// }
class PriceHistory {
 private:
  bool initialized;
  String tickerSymbol;
  ListList<tda::Candle> candles;
  ArrayList<tda::Candle> candleVector;
  StringMap priceHistoryVariables;
  StringMap candleData;

 public:
  PriceHistory();

  void addCandle(tda::Candle candle);
  void addCandleByType(Candle candle, int type);

  ArrayList<tda::Candle> getCandles(int index);
  int getNumCandles(int index);

  ArrayList<tda::Candle> getCandleVector();
  String getCandleDataVariable(String variable);
  String getPriceHistoryVariable(String variable);
  String getTickerSymbol();
  bool getInitialized();
  void setPriceHistoryVariable(String key, String value);
  void setTickerSymbol(String ticker);
  void setInitialized();
  void updatePriceHistory();
  void clear();
};
}  // namespace tda
}  // namespace Premia
#endif