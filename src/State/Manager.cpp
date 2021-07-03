//  Manager (State) Class
#include "Manager.hpp"
#include "State.hpp"

void Manager::init(int width, int height)
{
    m_running = true;

    if ( SDL_Init( SDL_INIT_EVERYTHING ) )
    {
        SDL_Log("SDL_Init: %s\n", SDL_GetError() );
    }
    else
    {
        pWindow = SDL_CreateWindow(
            "Premia",                           // window title
            SDL_WINDOWPOS_UNDEFINED,            // initial x position
            SDL_WINDOWPOS_UNDEFINED,            // initial y position
            width,                              // width, in pixels
            height,                             // height, in pixels
            SDL_WINDOW_RESIZABLE                // flags - see below
        );
        
        if ( pWindow == NULL )
        {
            SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
            SDL_Quit();
        }
        else
        {
            pRenderer = SDL_CreateRenderer( pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
            if( pRenderer == NULL )
            {
                SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
                SDL_Quit();
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( pRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    SDL_Log("Error initializing SDL_Image: %s\n", IMG_GetError());
                }
                
                if ( TTF_Init() == -1 )
                {
                    SDL_Log("Error initializing SDL_ttf: %s\n", TTF_GetError());
                }
            }

        }
    }
    ImGui_ImplSDL2_InitForPremia( pWindow );
}

void Manager::cleanup()
{
    while ( !states.empty() )
    {
        states.back()->cleanup();
        states.pop_back();
    }

    ImGuiSDL::Deinitialize();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // Close and destroy the window
    SDL_DestroyRenderer( pRenderer );
    SDL_DestroyWindow( pWindow );
    pWindow = NULL;
    pRenderer = NULL;

    ImGui::DestroyContext();
    
    // Final clean up
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

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
    states.back()->init( this->pRenderer, this->pWindow );
}

void Manager::push( State* state )
{
    // pause current state
    if ( !states.empty() ) {
        states.back()->pause();
    }

    // store and init the new state
    states.push_back(state);
    states.back()->init( this->pRenderer, this->pWindow );
}

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

void Manager::handleEvents()
{
    // let the state handle events
    states.back()->handleEvents(this);
}

void Manager::update()
{
    // let the state update the game
    states.back()->update(this);
}

void Manager::draw()
{
    // let the state draw the screen
    states.back()->draw(this);
}
