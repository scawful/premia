#ifndef LoginFrame_hpp
#define LoginFrame_hpp

#include "../../core.hpp"
#include "../Manager.hpp"

class LoginFrame 
{
private:
    bool isLoggedIn;

public:
    LoginFrame();

    void update();
};

#endif