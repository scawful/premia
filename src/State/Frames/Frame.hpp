#ifndef Frame_hpp
#define Frame_hpp

#include "../../core.hpp"
#include "../Manager.hpp"

class Frame
{
protected:
    Manager *premia;
    bool *tda_logged_in;

public:
    Frame() { }

    void import_manager(Manager *premia, bool *tda_logged_in = NULL) {
        this->premia = premia;
        this->tda_logged_in = tda_logged_in;
    }
    virtual void update() = 0;
};

#endif