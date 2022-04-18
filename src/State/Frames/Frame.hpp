#ifndef Frame_hpp
#define Frame_hpp

#include "../../core.hpp"
#include "../Manager.hpp"

class Frame
{
private:
    Manager *premia;
    bool *public_mode;
    bool *tda_logged_in;
    bool initialized;

protected:
    Manager * getPremia() const { return premia; }
    bool * getPublicMode() const { return public_mode; }
    bool * getTDALoggedIn() const { return tda_logged_in; }
    bool getInitialized() const { return initialized; }
    void setInitialized(bool init) { initialized = init; }


public:
    Frame()=default;
    ~Frame()=default;

    /**
     * @todo HOLY SHIT FIX THIS LOL
     * 
     * @param manager 
     * @param mode 
     * @param tda_login 
     */
    void import_manager(Manager * manager, bool * mode = nullptr, bool * tda_login = nullptr) {
        this->premia = manager;
        this->public_mode = mode;
        this->tda_logged_in = tda_login;
    }
    virtual void update() = 0;
};

#endif