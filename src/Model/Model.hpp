#ifndef Model_hpp
#define Model_hpp

#include "Premia.hpp"
#include "Services/TDA.hpp"

class Model 
{
private:
    std::unordered_map<String, EventHandler> callbacks;
    ConsoleLogger consoleLogger;
    
public:
    Model()=default;
    ~Model()=default;
    

    void addLogger(const ConsoleLogger & logger);
    void addEventHandler(CRString key, const EventHandler & handler);

    ConsoleLogger & getLogger();
};

#endif