#ifndef ChartFrame_hpp
#define ChartFrame_hpp

#include "../../core.hpp"
#include "Frame.hpp"

class CandleChartFrame : public Frame
{
private:
    // TDAmeritrade Objects
    tda::Quote quote;
    tda::PriceHistory price_history_data;
    std::vector<int> volume;
    std::vector<tda::Candle> candles;
    std::unordered_map< std::string, tda::Quote> quotes;
    std::string detailed_quote, title_string, ticker_symbol;

    // Initialization functions
    void init_candles();
    void init_quote_details();

    // CandleChart
    int binary_search( const double* arr, int l, int r, double x );
    void build_candle_chart( float width_percent, int count, ImVec4 bullCol, ImVec4 bearCol, bool tooltip );

public:
    CandleChartFrame();

    void init_instrument( std::string ticker );
    void update();
};

#endif