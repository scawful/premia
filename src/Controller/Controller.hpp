#ifndef Controller_hpp
#define Controller_hpp

#include "../core.hpp"
#include "../Model/PrimaryModel.hpp"
#include "../View/LoginView.hpp"

class Controller
{
public:
    Controller()=default;
    virtual ~Controller()=default;

    virtual void onLoad() = 0;
};

#endif