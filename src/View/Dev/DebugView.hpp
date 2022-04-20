#ifndef DebugView_hpp
#define DebugView_hpp

#include "../View.hpp"

class DebugView : public View
{
private:
    bool initialized = false;
    std::vector<VoidEventHandler> events;

    void drawScreen() const;

public:
    void addEvent(const VoidEventHandler & event) override;
    void update() override;
};

#endif