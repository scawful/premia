#ifndef LoginFrame_hpp
#define LoginFrame_hpp

#include "Frame.hpp"

class LoginFrame : public Frame
{
private:
    bool isLoggedIn;

public:
    LoginFrame();

    bool checkLoginStatus() const;
    void update();
};

#endif