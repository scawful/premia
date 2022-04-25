#ifndef ChartFrame_hpp
#define ChartFrame_hpp

#include "Frame.hpp"

class CandleChartFrame : public Frame
{
private:
    // TDAmeritrade Objects
    tda::Quote quote;
    tda::PriceHistory priceHistoryData;
    std::vector<tda::Candle> candles;
    std::unordered_map< std::string, tda::Quote> quotes;
    std::string detailed_quote;
    std::string title_string;
    std::string ticker_symbol;

    // Current Chart Arrays
    double *dates;
    double *highs;
    double *lows;
    double *opens;
    double *closes;
    double *volume;

    char buf[64] = "";

    // Helper functions 
    int binary_search( const double* arr, int l, int r, double x );

    // Initialization functions
    void init_candles(int index);

    // Candle Chart
    void build_candle_chart( float width_percent, int count, ImVec4 bullCol, ImVec4 bearCol, bool tooltip );
    void draw_chart();

public:
    CandleChartFrame();
    ~CandleChartFrame();

    void init_instrument( std::string ticker );
    void update();
};

#endif