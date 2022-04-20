#ifndef PrimaryModel_hpp
#define PrimaryModel_hpp

#include "Model.hpp"

class PrimaryModel: public Model 
{
private:
    std::unordered_map<std::string, VoidEventHandler> callbacks;

    tda::TDAmeritrade TDAInterface;
    cbp::CoinbasePro CoinbaseProInterface;
    iex::IEXCloud IEXCloudInterface;
    halext::Halext HalextInterface;
    
public:
    void login(const std::string & username, const std::string & password);
    void addEventHandler(const std::string & key, const VoidEventHandler & handler) {
        this->callbacks[key] = handler;
    }
};

#endif