#include "PriceHistory.hpp"

using namespace tda;

/* =============== PriceHistory Class =============== */

void PriceHistory::initVariables()
{
    for ( auto& history_it: priceHistoryData )
    {
        if ( history_it.first == "candles" )
        {
            for ( auto& candle_it: history_it.second )
            {
                tda::Candle newCandle;
                std::string datetime;
                std::pair<double, double> high_low;
                std::pair<double, double> open_close;

                for ( auto& candle2_it: candle_it.second )
                {
                    if ( candle2_it.first == "high" )
                        high_low.first = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                    if ( candle2_it.first == "low" )
                        high_low.second = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                    
                    if ( candle2_it.first == "open" )
                        open_close.first = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                    if ( candle2_it.first == "close" )
                        open_close.second = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );

                    if ( candle2_it.first == "volume" )
                        newCandle.volume = stoi( candle2_it.second.get_value<std::string> () );
                        
                    if ( candle2_it.first == "datetime" )
                    {
                        std::stringstream dt_ss;
                        std::time_t secsSinceEpoch = boost::lexical_cast<std::time_t>(candle2_it.second.get_value<std::string> ());
                        newCandle.raw_datetime = secsSinceEpoch;
                        secsSinceEpoch *= 0.001;

                        //%a %d %b %Y - %I:%M:%S%p
                        //%H:%M:%S
                        dt_ss << std::put_time(std::localtime(&secsSinceEpoch), "%a %d %b %Y - %I:%M:%S%p");
                        datetime = dt_ss.str();
                        //datetime = boost::lexical_cast<std::string>(candle2_it.second.get_value<std::string> ());
                    }
                    
                }

                newCandle.datetime = datetime;
                newCandle.highLow = high_low;
                newCandle.openClose = open_close;

                candleVector.push_back( newCandle );
            }
        }
        else if ( history_it.first == "symbol" )
        {
            priceHistoryVariables["symbol"] = history_it.second.get_value<std::string>();
        }
    }
}

PriceHistory::PriceHistory() 
{
    
}

PriceHistory::PriceHistory( boost::property_tree::ptree price_history_data )
{
    priceHistoryData = price_history_data;
    initVariables();
}

void PriceHistory::addCandle(tda::Candle candle)
{
    candleVector.push_back(candle);
}

std::vector< tda::Candle > PriceHistory::getCandleVector()
{
    return candleVector;
}

std::string PriceHistory::getCandleDataVariable( std::string variable )
{
    return candleData[ variable ];
}

std::string PriceHistory::getPriceHistoryVariable( std::string variable )
{
    return priceHistoryVariables[variable];
}

void PriceHistory::setPriceHistoryVariable(std::string key, std::string value)
{
    priceHistoryVariables[key] = value;
}

void PriceHistory::clear()
{
    priceHistoryVariables.clear();
    candleVector.clear();
}