#ifndef Model_hpp
#define Model_hpp

#include "Premia.hpp"
#include "../Services/TDAmeritrade.hpp"

class Model 
{
private:
    std::unordered_map<String, Premia::EventHandler> callbacks;
    Premia::ConsoleLogger consoleLogger;

    tda::TDAmeritrade TDAInterface;
    
public:
    Model()=default;
    ~Model()=default;
    

    void addAuth(CRString, CRString);
    void addLogger(const Premia::ConsoleLogger & logger);
    void addEventHandler(CRString key, const Premia::EventHandler & handler);

    Premia::ConsoleLogger & getLogger();

    tda::TDAmeritrade & getTDAInterface();
};

#endif