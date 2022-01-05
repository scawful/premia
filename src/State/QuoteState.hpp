#ifndef QuoteState_hpp
#define QuoteState_hpp

#include "State.hpp"

class QuoteState : public State
{
private:
    static QuoteState m_QuoteState;

    Manager *premia = NULL;

    std::string detailed_quote, title_string, ticker_symbol;

    tda::Quote quoteData;
    tda::PriceHistory priceHistoryData;

    std::unordered_map< std::string, tda::Quote> quotes;

    std::vector<tda::Candle> candleVector;
    std::vector<int> volumeVector;

    // Functions
    void initCandleArrays();
    void setDetailedQuote( std::string ticker );

protected:
    QuoteState() { }

public:
    void init(Manager *premia);
    void cleanup();

    void pause();
    void resume();

    void setQuote( std::string ticker );
    void createCandleChart( float width_percent, int count, ImVec4 bullCol, ImVec4 bearCol, bool tooltip );

    void handleEvents();
    void update();
    void draw();

    static QuoteState* instance()
    {
        return &m_QuoteState;
    }

};

#endif