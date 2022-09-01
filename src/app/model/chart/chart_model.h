#ifndef ChartModel_hpp
#define ChartModel_hpp

#include <string>
#include <vector>

#include "model/model.h"
#include "service/TDAmeritrade/client.h"

namespace premia {
  
class ChartModel : public Model {
 public:
  auto getDate(int i) { return datesVec.at(i); }
  auto getDates() const { return datesVec; }
  auto getVolumeVector() const { return volumeVec; }
  auto isActive() const { return active; }
  auto getNumCandles() const { return (int)candles.size(); }
  auto getCandle(int i) { return candles.at(i); }
  auto getTickerSymbol() const { return tickerSymbol; }
  tda::Quote& getQuote() { return quote; }

  std::string getQuoteDetails();
  void fetchPriceHistory(const std::string& ticker, tda::PeriodType ptype,
                         int period_amt, tda::FrequencyType ftype, int freq_amt,
                         bool ext);

 private:
  void initCandles();

  bool active = false;
  std::string tickerSymbol;
  SocketListener socketListener;
  tda::Quote quote;
  tda::PriceHistory priceHistory;
  std::vector<tda::Candle> candles;
  std::vector<double> datesVec;
  std::vector<double> volumeVec;
};
}  // namespace premia
#endif