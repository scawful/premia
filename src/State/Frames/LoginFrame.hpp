#ifndef LoginFrame_hpp
#define LoginFrame_hpp

#include "Frame.hpp"

class LoginFrame : public Frame
{
private:
    bool isLoggedIn;
    unsigned attempt = 0;
    std::string connectOptions;
    std::string host;
    int port;
    int clientId;

public:
    LoginFrame();

    void update();
};

#endif