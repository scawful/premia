//  Manager (State) Class Header

#ifndef Manager_hpp
#define Manager_hpp

#include "../core.hpp"
#include "../Services/TDAmeritrade.hpp"
#include "../Services/CoinbasePro.hpp"
#include "../Services/IEXCloud.hpp"
// #include "../Services/InteractiveBrokers.hpp"
#include "../Services/KuCoin.hpp"

class State;

class Manager
{
private:
    std::vector<State*> states;    
    bool m_running;
    
public:
    void init(int width = SCREEN_WIDTH, int height = SCREEN_HEIGHT);
    void cleanup();
    
    void change(State *state);
    void push(State *state);
    void pop();
    
    void updateDelta();
    void updateMousePosition();
    void handleEvents();
    void update();
    void draw();
    
    bool running() { return m_running; }
    void quit() { m_running = false; }
    
    tda::TDAmeritrade tda_client;
    iex::IEXCloud iex_client;
    kc::KuCoin kucoin_interface;
    // InteractiveBrokers ibkr_client;
    
    SDL_Renderer *pRenderer = NULL;
    SDL_Window *pWindow = NULL;
    SDL_Surface *screen;

};

#endif /* Manager_hpp */
