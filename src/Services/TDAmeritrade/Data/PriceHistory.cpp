#include "PriceHistory.hpp"

using namespace tda;

PriceHistory::PriceHistory() 
{
    this->tickerSymbol = "";
    this->initialized = false;
    for (int i = 0; i < 4; i++ ) {
        ArrayList<tda::Candle> vector;
        candles.push_back(vector);
    }
}

void PriceHistory::addCandle(tda::Candle candle)
{
    candleVector.push_back(candle);
}

void PriceHistory::addCandleByType(Candle candle, int type)
{
    candles[type].push_back(candle);
}

ArrayList<tda::Candle> PriceHistory::getCandles(int index)
{
    return candles[index];
}

int PriceHistory::getNumCandles(int index)
{
    return candles[index].size();
}

ArrayList< tda::Candle > PriceHistory::getCandleVector()
{
    return candleVector;
}

String PriceHistory::getCandleDataVariable( String variable )
{
    return candleData[ variable ];
}

String PriceHistory::getPriceHistoryVariable( String variable )
{
    return priceHistoryVariables[variable];
}

String PriceHistory::getTickerSymbol()
{
    return tickerSymbol;
}

bool PriceHistory::getInitialized()
{
    return initialized;
}

void PriceHistory::setPriceHistoryVariable(String key, String value)
{
    priceHistoryVariables[key] = value;
}

void PriceHistory::setTickerSymbol(String ticker)
{
    this->tickerSymbol = ticker;
}

void PriceHistory::setInitialized()
{
    this->initialized = true;
}

void PriceHistory::clear()
{
    priceHistoryVariables.clear();
    candleVector.clear();
}