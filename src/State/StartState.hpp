//  StartState Class Header
#ifndef StartState_hpp
#define StartState_hpp

#include "State.hpp"

class StartState : public State
{
private:
    static StartState m_StartState;

    Manager *premia = NULL;
    SDL_Texture *pTexture = NULL;
    TTF_Font *titleFont = NULL;
    TTF_Font *menuFont = NULL;
    TTF_Font *priceFont = NULL;

    std::map<std::string, tda::Quote > quotes;

protected:
    StartState() { }

public:
    void init(Manager *premia);
    void cleanup();

    void pause();
    void resume();

    void handleEvents();
    void update();
    void draw();

    static StartState* instance()
    {
        return &m_StartState;
    }

};

#endif /* StartState_hpp */
