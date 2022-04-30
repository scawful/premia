#ifndef DebugView_hpp
#define DebugView_hpp

#include "../View.hpp"

class DebugView : public View
{
private:
    bool initialized = false;
    std::unordered_map<std::string, VoidEventHandler> events;
    ConsoleLogger logger;

    void drawScreen() const;

public:
    void addAuth(const std::string & key , const std::string & token) override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void update() override;
};

#endif