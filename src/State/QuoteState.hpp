#ifndef QuoteState_hpp
#define QuoteState_hpp

#include "State.hpp"

class QuoteState : public State
{
private:
    static QuoteState m_QuoteState;

    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;
    TTF_Font *tickerFont = NULL;

    PTexture tickerTexture;
    PTexture descTexture;

    std::string detailed_quote, title_string, ticker_symbol;

    boost::shared_ptr<tda::TDAmeritrade> tda_data_interface;
    boost::shared_ptr<tda::Quote> quoteData;
    boost::shared_ptr<tda::PriceHistory> priceHistoryData;

    std::vector< tda::Candle > candleVector;
    std::vector< int > volumeVector;

protected:
    QuoteState() { }

public:
    void init( SDL_Renderer *pRenderer, SDL_Window *pWindow );
    void cleanup();

    void pause();
    void resume();

    void setQuote( std::string ticker );
    void createCandleChart( float width_percent, int count, ImVec4 bullCol, ImVec4 bearCol, bool tooltip );

    void handleEvents( Manager* premia );
    void update( Manager* premia );
    void draw( Manager* premia );

    static QuoteState* instance()
    {
        return &m_QuoteState;
    }

};

#endif