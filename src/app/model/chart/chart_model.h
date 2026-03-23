#ifndef ChartModel_hpp
#define ChartModel_hpp

#include <string>
#include <vector>

#include "model/model.h"
#include "premia/core/application/screen_models.hpp"
#include "service/TDAmeritrade/data/PricingStructures.hpp"

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
  auto getLowBound() const -> double { return lowBound; }
  auto getHighBound() const -> double { return highBound; }

  std::string getQuoteDetails();
  void fetchPriceHistory(const std::string& ticker, tda::PeriodType ptype,
                         int period_amt, tda::FrequencyType ftype, int freq_amt,
                         bool ext);

 private:
  void initCandles();

  bool active = false;
  std::string tickerSymbol;
  SocketListener socketListener;
  core::application::QuoteDetail quoteDetail;
  core::application::ChartScreenData chartData;
  std::vector<tda::Candle> candles;
  std::vector<double> datesVec;
  std::vector<double> volumeVec;
  double lowBound = 0.0;
  double highBound = 0.0;
};
}  // namespace premia
#endif
