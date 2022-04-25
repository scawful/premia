//  Manager (State) Class
#include "Manager.hpp"
#include "State.hpp"

/**
 * @brief Initialize SDL components with ImGui backend
 * @author @scawful
 * 
 * @param width 
 * @param height 
 */
void Manager::init(int width, int height)
{
    m_running = true;

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError() );
    } else {
        pWindow = SDL_CreateWindow(
            "Premia",                           // window title
            SDL_WINDOWPOS_UNDEFINED,            // initial x position
            SDL_WINDOWPOS_UNDEFINED,            // initial y position
            width,                              // width, in pixels
            height,                             // height, in pixels
            SDL_WINDOW_RESIZABLE                // flags
        );
    }   

    if (pWindow == nullptr) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
    }
    else
    {
        pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (pRenderer == nullptr)
        {
            SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
            SDL_Quit();
        }
        else
        {
            // Initialize renderer color
            SDL_SetRenderDrawColor(pRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

            // Initialize PNG loading
            if (int imgFlags = IMG_INIT_PNG; !(IMG_Init( imgFlags ) & imgFlags)) {
                SDL_Log("Error initializing SDL_Image: %s\n", IMG_GetError());
            }
            
            if ( TTF_Init() == -1 ) {
                SDL_Log("Error initializing SDL_ttf: %s\n", TTF_GetError());
            }
        }
    }


    // Create the ImGui context 
    ImGui::CreateContext();
    ImPlot::CreateContext();

    // Initialize ImGui for SDL 
    ImGui_ImplSDL2_InitForSDLRenderer(pWindow, pRenderer);
    ImGui_ImplSDLRenderer_Init(pRenderer);

    // Build a new ImGui frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(pWindow);
}

/**
 * @brief Cleanup state data, ImGui context, ImPlot context, and SDL data
 * @author @scawful
 * 
 */
void Manager::cleanup()
{
    // Clear all the states 
    while ( !states.empty() )
    {
        states.back()->cleanup();
        states.pop_back();
    }

    // Shut down SDL renderer 
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    // Close and destroy the window
    SDL_DestroyRenderer( pRenderer );
    SDL_DestroyWindow( pWindow );
    pWindow = nullptr;
    pRenderer = nullptr;

    // Final clean up
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

/**
 * @brief Change the current state handled by the Manager
 * @author @scawful
 * 
 * @param state 
 */
void Manager::change( State* state )
{
    // cleanup the current state
    if ( !states.empty() ) 
    {
        states.back()->cleanup();
        states.pop_back();
    }

    // store and init the new state
    states.push_back(state);
    states.back()->init( this );
}

/**
 * @brief Push a new state to the stack
 * @author @scawful
 * 
 * @param state 
 */
void Manager::push( State* state )
{
    // pause current state
    if ( !states.empty() ) {
        states.back()->pause();
    }

    // store and init the new state
    states.push_back(state);
    states.back()->init( this );
}

/**
 * @brief Pop a state off of the stack
 * @author @scawful
 * 
 */
void Manager::pop()
{
    // cleanup the current state
    if ( !states.empty() ) {
        states.back()->cleanup();
        states.pop_back();
    }

    // resume previous state
    if ( !states.empty() ) {
        states.back()->resume();
    }
}

/**
 * @brief Handle any events in the current state
 * @author @scawful
 * 
 */
void Manager::handleEvents()
{
    // let the state handle events
    states.back()->handleEvents();
}

/**
 * @brief Update the current state
 * @author @scawful
 * 
 */
void Manager::update()
{
    // let the state update the game
    states.back()->update();
}

/**
 * @brief Draw the contents of the current state
 * @author @scawful
 * 
 */
void Manager::draw()
{
    // let the state draw the screen
    states.back()->draw();
}
