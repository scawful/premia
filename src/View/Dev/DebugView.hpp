#ifndef DebugView_hpp
#define DebugView_hpp

#include "../View.hpp"

class DebugView : public View
{
private:
    bool initialized = false;
    std::unordered_map<std::string, VoidEventHandler> events;

    void drawScreen() const;

public:
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void update() override;
};

#endif