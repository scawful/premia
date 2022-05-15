#ifndef Model_hpp
#define Model_hpp

#include "Premia.hpp"
#include "Interface/TDA.hpp"

class Model 
{
private:
    std::unordered_map<String, EventHandler> callbacks;
    Logger consoleLogger;
    
public:
    Model()=default;
    ~Model()=default;
    

    void addLogger(const Logger& logger);
    void addEventHandler(CRString key, const EventHandler & handler);

    Logger& getLogger();
};

#endif