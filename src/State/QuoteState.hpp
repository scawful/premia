#ifndef QuoteState_hpp
#define QuoteState_hpp

#include "State.hpp"

class QuoteState : public State
{
private:
    static QuoteState m_QuoteState;

    Manager *premia = NULL;

    std::string detailed_quote, title_string, ticker_symbol;

    boost::shared_ptr<tda::TDAmeritrade> tda_data_interface;
    boost::shared_ptr<tda::Quote> quoteData;
    boost::shared_ptr<tda::PriceHistory> priceHistoryData;

    std::unordered_map< std::string, boost::shared_ptr<tda::Quote> > quotes;

    std::vector< tda::Candle > candleVector;
    std::vector< int > volumeVector;

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