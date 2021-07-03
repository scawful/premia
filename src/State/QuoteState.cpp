//  QuoteState Class
#include "QuoteState.hpp"
#include "StartState.hpp"

QuoteState QuoteState::m_QuoteState;

void QuoteState::init( SDL_Renderer *pRenderer, SDL_Window *pWindow )
{
    this->pRenderer = pRenderer;
    this->pWindow = pWindow;
    this->tda_data_interface = boost::make_shared<tda::TDAmeritrade>(tda::QUOTE);
    this->setQuote( "TLT" );

    SDL_Color fontColor = { 255, 255, 255 };
    tickerFont = TTF_OpenFont( "../assets/arial.ttf", 52 );
    tickerTexture.loadFromRenderedText( pRenderer, tickerFont, this->quoteData->getQuoteVariable("symbol"), fontColor );

    TTF_CloseFont( tickerFont );
    tickerFont = TTF_OpenFont( "../assets/arial.ttf", 32 );
    descTexture.loadFromRenderedText( pRenderer, tickerFont, this->quoteData->getQuoteVariable("description"), fontColor );

}

void QuoteState::cleanup()
{
    TTF_CloseFont( tickerFont );

    printf("QuoteState Cleanup\n");
}

void QuoteState::pause()
{
    printf("QuoteState Pause\n");
}

void QuoteState::resume()
{
    printf("QuoteState Resume\n");
}

void QuoteState::setQuote( std::string ticker )
{
    this->quoteData = tda_data_interface->createQuote( ticker );
}

void QuoteState::handleEvents( Manager* premia )
{
    SDL_Event event;
    SDL_Color fontColor = { 255, 255, 255 };

    while ( SDL_PollEvent(&event) ) 
    {
        switch ( event.type ) 
        {
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym )
                {
                    case SDLK_ESCAPE:
                        premia->quit();
                        break;
                    case SDLK_SPACE:
                        premia->change( StartState::instance() );
                        break;
                    default:
                        break;
                }

            case SDL_WINDOWEVENT:
                switch ( event.window.event ) 
                {
                    case SDL_WINDOWEVENT_CLOSE:   // exit game
                        premia->quit();
                        break;

                    default:
                        break;
                }
                break;      

            default:
                break;
        }
    }

}

void QuoteState::update( Manager* game )
{
    SDL_RenderClear( this->pRenderer );
}

void QuoteState::draw( Manager* game )
{
    // fill window bounds
    int w = 1920, h = 1080;
    SDL_SetRenderDrawColor( this->pRenderer, 55, 55, 55, 0 );
    SDL_GetWindowSize( this->pWindow, &w, &h );
    SDL_Rect f = {0, 0, 1920, 1080};
    SDL_RenderFillRect( this->pRenderer, &f );

    tickerTexture.render( pRenderer, 10, 0 );
    descTexture.render( pRenderer, 10, 50 );

    SDL_RenderPresent( this->pRenderer );
}
