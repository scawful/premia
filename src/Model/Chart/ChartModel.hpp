#ifndef ChartModel_hpp
#define ChartModel_hpp

#include "../Model.hpp"
namespace Premia {
class ChartModel : public Model {
 private:
  bool active = false;
  String tickerSymbol;
  SocketListener socketListener;
  tda::Quote quote;
  tda::PriceHistory priceHistory;
  ArrayList<tda::Candle> candles;
  ArrayList<double> datesVec;
  ArrayList<double> volumeVec;

  void initCandles();
  void initListener();

 public:
  bool isActive() const;
  ArrayList<double> getDates() const;
  ArrayList<double> getVolumeVector() const;
  double getDate(int i);
  tda::Quote& getQuote();
  int getNumCandles() const;
  tda::Candle getCandle(int i);
  String getTickerSymbol() const;
  String getQuoteDetails();
  void fetchPriceHistory(CRString ticker, tda::PeriodType ptype, int period_amt,
                         tda::FrequencyType ftype, int freq_amt, bool ext);
};
}  // namespace Premia
#endif