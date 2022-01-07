#ifndef MenuFrame_hpp
#define MenuFrame_hpp

#include "../../core.hpp"
#include "../Manager.hpp"

class MenuFrame 
{
private:
    Manager *premia;
    std::string title_string;

public:
    MenuFrame();

    void import_manager(Manager *premia);
    void set_title(std::string & title_string);
    void update();
    void render();

};


#endif