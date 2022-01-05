//  Manager (State) Class Header

#ifndef Manager_hpp
#define Manager_hpp

#include "../core.hpp"
#include "../MarketData/TDAmeritrade.hpp"
#include "../MarketData/CoinbasePro.hpp"

class State;

class Manager
{
private:
    std::vector<State*> states;
    boost::shared_ptr<tda::TDAmeritrade> tdaDataInterface;
    
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
    
    SDL_Renderer *pRenderer = NULL;
    SDL_Window *pWindow = NULL;
    SDL_Surface *screen;

};

#endif /* Manager_hpp */
