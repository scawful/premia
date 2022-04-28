#include "ChartModel.hpp"

void ChartModel::initCandles()
{
    candles = priceHistory.getCandleVector();
    int numCandles = getNumCandles();
    for ( int i = 0; i < numCandles; i++ )
    {
        double new_dt = 0.0;
        try {
            new_dt = boost::lexical_cast<double>(candles[i].raw_datetime);
        } catch (boost::wrapexcept<boost::bad_lexical_cast>& e) {
            std::cout << e.what() << std::endl;
        }
        
        new_dt *= 0.001;
        datesVec.push_back(new_dt);
    }
}

std::vector<double> ChartModel::getDates()
{
    return datesVec;
}

double ChartModel::getDate(int i)
{
    return datesVec.at(i);
}

bool ChartModel::isActive() const
{
    return active;
}

tda::Quote & ChartModel::getQuote()
{
    return quote;
}

int ChartModel::getNumCandles() const
{
    return candles.size();
}

tda::Candle ChartModel::getCandle(int i)
{
    return candles.at(i);
}

std::string ChartModel::getTickerSymbol() const
{
    return tickerSymbol;
}

const std::string ChartModel::getQuoteDetails()
{
    std::string quoteDetails = "Bid: $" + quote.getQuoteVariable("bidPrice") + " - Size: " + quote.getQuoteVariable("bidSize") +
                                 "\nAsk: $" + quote.getQuoteVariable("askPrice") + " - Size: " + quote.getQuoteVariable("askSize") +
                                 "\nOpen: $" + quote.getQuoteVariable("openPrice") + " | Close: $" + quote.getQuoteVariable("closePrice") +
                                 "\n52 Week High: $" + quote.getQuoteVariable("52WkHigh") + " | 52 Week Low: $" + quote.getQuoteVariable("52WkLow") +
                                 "\nTotal Volume: " + quote.getQuoteVariable("totalVolume") + " | Exchange: " + quote.getQuoteVariable("exchangeName");
    return quoteDetails;
}

void ChartModel::fetchPriceHistory(const std::string & ticker, tda::PeriodType ptype, int period_amt, tda::FrequencyType ftype, int freq_amt, bool ext)
{
    if (active) {
        quote.clear();
        priceHistory.clear();
        datesVec.clear();
    }
    tickerSymbol = ticker;
    quote = getTDAInterface().getQuote(ticker);
    priceHistory = getTDAInterface().getPriceHistory(ticker, ptype, period_amt, ftype, freq_amt, ext);
    initCandles();
    active = true;
}

