#include "State/StartState.hpp"

int main( int argc, char* argv[] )
{
    Manager premia;
    
    // QuantLib::Option::Type OptionType(QuantLib::Option::Call);
    // std::cout << "Option Type = " << OptionType << std::endl;

    // tda::TDAmeritrade tda_api_interface(tda::QUOTE);
    // tda_api_interface.retrieveQuoteData( "TLT", false );

    premia.init();
    premia.change( StartState::instance() );
    
    while ( premia.running() )
    {
        premia.handleEvents();
        premia.update();
        premia.draw();
    }

    // cleanup the engine
    premia.cleanup();

    return 0;
}