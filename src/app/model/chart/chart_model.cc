#include "chart_model.h"

namespace premia {

void ChartModel::initCandles() {
  std::ifstream keyfile("assets/apikey.txt");
  std::string consumer_key;
  std::string refresh_token;
  if (keyfile.good()) {
    std::stringstream buffer;
    buffer << keyfile.rdbuf();
    buffer >> consumer_key;
    buffer >> refresh_token;
    keyfile.close();
  }
  tda::TDA::getInstance().authUser(consumer_key, refresh_token);
  candles = priceHistory.getCandleVector();
  int numCandles = getNumCandles();
  for (int i = 0; i < numCandles; i++) {
    double new_dt = 0.0;
    double volume = 0.0;
    try {
      new_dt = boost::lexical_cast<double>(candles[i].raw_datetime);
      volume = candles[i].volume;
    } catch (const boost::wrapexcept<boost::bad_lexical_cast>& e) {
      getLogger()(e.what());
    }

    new_dt *= 0.001;
    datesVec.push_back(new_dt);
    volumeVec.push_back(volume);
  }
}

std::string ChartModel::getQuoteDetails() {
  std::string quoteDetails =
      "Bid: $" + quote.getQuoteVariable("bidPrice") + " | Ask: $" +
      quote.getQuoteVariable("askPrice") + " | Open: $" +
      quote.getQuoteVariable("openPrice") + " | Close: $" +
      quote.getQuoteVariable("closePrice");
  return quoteDetails;
}

void ChartModel::fetchPriceHistory(const std::string& ticker,
                                   tda::PeriodType ptype, int period_amt,
                                   tda::FrequencyType ftype, int freq_amt,
                                   bool ext) {
  if (active) {
    quote.clear();
    priceHistory.clear();
    datesVec.clear();
  }
  tickerSymbol = ticker;
  quote = tda::TDA::getInstance().getQuote(ticker);
  priceHistory = tda::TDA::getInstance().getPriceHistory(
      ticker, ptype, ftype, period_amt, freq_amt, ext);
  initCandles();
  active = true;
}
}  // namespace premia