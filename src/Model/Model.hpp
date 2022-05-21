#ifndef Model_hpp
#define Model_hpp

#include "Premia.hpp"
#include "Interface/TDA.hpp"
#include "Interface/TWS.hpp"

class Model 
{
private:
    EventMap callbacks;
    Logger consoleLogger;
    
public:
    Model()=default;
    ~Model()=default;

    void addLogger(const Logger& logger);
    void addEventHandler(CRString key, const EventHandler & handler);

    Logger& getLogger();
};

#endif