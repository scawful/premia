//  Manager (State) Class Header

#ifndef Manager_hpp
#define Manager_hpp

#include "../core.hpp"
#include "../Services/GenericClient.hpp"
#include "../Services/TDAmeritrade.hpp"
#include "../Services/IEXCloud.hpp"
#include "../Services/CoinbasePro/CoinbasePro.hpp"
#include "../Services/Halext/Halext.hpp"

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
    
    bool running() const { return m_running; }
    void quit() { m_running = false; }
    
    halext::Halext halext_interface;
    tda::TDAmeritrade tda_interface;
    iex::IEXCloud iex_interface;
    GenericClient client;
    
    SDL_Renderer * pRenderer = nullptr;
    SDL_Window * pWindow = nullptr;
    SDL_Surface * screen;
};

#endif /* Manager_hpp */
