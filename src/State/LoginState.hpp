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
    bool protectedMode, tdaLoggedIn;

    Manager * premia = nullptr;
    LoginFrame loginFrame;

protected:
    LoginState()=default;
    ~LoginState()=default;

public:
    void init(Manager *premia) override;
    void cleanup()             override;

    void pause()               override;
    void resume()              override;

    void handleEvents()        override;
    void update()              override;
    void draw()                override;

    static LoginState* instance()
    {
        return &m_LoginState;
    }

};

#endif /* LoginState_hpp */
