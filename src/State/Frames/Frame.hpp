#ifndef Frame_hpp
#define Frame_hpp

#include "../../core.hpp"
#include "../Manager.hpp"

class Frame
{
protected:
    Manager *premia;

public:
    Frame() { }

    void import_manager(Manager *premia) {
        this->premia = premia;
    }
    virtual void update() = 0;
};

#endif