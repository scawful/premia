#ifndef Frame_hpp
#define Frame_hpp

#include "core.hpp"
#include "../Manager.hpp"

class Frame
{
private:
    bool initialized;

protected:
    Manager * premia;
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
    void import_manager(Manager * manager) {
        this->premia = manager;
    }
    virtual void update() = 0;
};

#endif