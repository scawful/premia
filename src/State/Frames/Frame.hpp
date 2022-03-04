#ifndef Frame_hpp
#define Frame_hpp

#include "../../core.hpp"
#include "../Manager.hpp"

class Frame
{
protected:
    Manager *premia;
    bool *public_mode;
    bool *tda_logged_in;
    bool initialized;

public:
    Frame() { }

    void import_manager(Manager *premia, bool *public_mode = NULL, bool *tda_logged_in = NULL) {
        this->premia = premia;
        this->public_mode = public_mode;
        this->tda_logged_in = tda_logged_in;
    }
    virtual void update() = 0;
};

#endif