//  Manager (State) Class Header

#ifndef Manager_hpp
#define Manager_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../libs/imgui/imgui.h"
#include "../libs/imgui/backends/imgui_impl_sdl.h"
#include "../libs/imgui/backends/imgui_impl_sdlrenderer.h"
#include "../libs/imgui/misc/cpp/imgui_stdlib.h"
#include "../libs/implot/implot.h"
#include "../libs/implot/implot_internal.h"
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

    halext::Halext halext_interface;
    iex::IEXCloud iex_interface;
    GenericClient client;

    SDL_Renderer * pRenderer = nullptr;
    SDL_Window * pWindow = nullptr;
    SDL_Surface * screen;

public:
    void init(int width = Premia::SCREEN_WIDTH, int height = Premia::SCREEN_HEIGHT);
    void cleanup();
    
    void change(State * state);
    void push(State * state);
    void pop();
    
    void updateDelta();
    void updateMousePosition();
    void handleEvents();
    void update();
    void draw();
    
    bool running() const { return m_running; }
    void quit() { m_running = false; }

    tda::TDAmeritrade tda_interface;
    halext::Halext getHalextInterface() const { return halext_interface; }
    iex::IEXCloud getIEXInterface()     const { return iex_interface; }
    GenericClient getGenericClient()    const { return client; };
    SDL_Renderer * getRenderer()        const { return pRenderer; }
    SDL_Window * getWindow()            const { return pWindow; }
    SDL_Surface * getScreen()           const { return screen; }
};

#endif /* Manager_hpp */
