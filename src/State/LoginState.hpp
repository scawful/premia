//  PrimaryState Class Header
#ifndef LoginState_hpp
#define LoginState_hpp

#include "State.hpp"
#include "PrimaryState.hpp"
#include "Frames/LoginFrame.hpp"

class LoginState : public State
{
private:
    static LoginState m_LoginState;

    Manager* premia = NULL;
    LoginFrame loginFrame;

protected:
    LoginState() { }

public:
    void init(Manager *premia);
    void cleanup();

    void pause();
    void resume();

    void handleEvents();
    void update();
    void draw();

    static LoginState* instance()
    {
        return &m_LoginState;
    }

};

#endif /* LoginState_hpp */
