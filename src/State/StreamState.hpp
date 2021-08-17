#ifndef StreamState_hpp
#define StreamState_hpp

#include "State.hpp"

class StreamState : public State
{
private:
    static StreamState m_StreamState;

    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;

    std::string title_string, ticker_symbol;

    boost::shared_ptr<tda::TDAmeritrade> tda_data_interface;

    bool request_fields[53];
    const char *quote_fields[53] = { "Symbol", "Bid Price", "Ask Price", "Last Price", "Bid Size",
    "Ask Size", "Ask ID", "Bid ID", "Total Volume", "Last Size", "Trade Time", "Quote Time",
    "High Price", "Low Price", "Bid Tick", "Close Price", "Exchange ID", "Marginable", "Shortable",
    "Island Bid", "Island Ask", "Island Volume", "Quote Day", "Trade Day", "Volatility", "Description",
    "Last ID", "Digits", "Open Price", "Net Change", "52 Week High", "52 Week Low", "PE Ratio", "Dividend Amount",
    "Dividend Yield", "Island Bid Size", "Island Ask Size", "NAV", "Fund Price", "Exchange Name", "Dividend Date",
    "Regular Market Quote", "Regular Market Trade", "Regular Market Last Price", "Regular Market Last Size",
    "Regular Market Trade Time", "Regular Market Trade Day", "Regular Market Net Change", "Security Status",
    "Mark", "Quote Time in Long", "Trade Time in Long", "Regular Market Trade Time in Long" };

    void set_instrument( std::string ticker );

protected:
    StreamState() { }

public:
    void init( SDL_Renderer *pRenderer, SDL_Window *pWindow );
    void cleanup();

    void pause();
    void resume();


    void handleEvents( Manager* premia );
    void update( Manager* premia );
    void draw( Manager* premia );

    static StreamState* instance()
    {
        return &m_StreamState;
    }
};

#endif