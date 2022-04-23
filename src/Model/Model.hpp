#ifndef Model_hpp
#define Model_hpp

#include "../core.hpp"
#include "../Services/TDAmeritrade.hpp"
#include "../Services/GenericClient.hpp"
#include "../Services/IEXCloud.hpp"
#include "../Services/Halext/Halext.hpp"
#include "../Services/CoinbasePro/CoinbasePro.hpp"

class Model 
{
private:
    std::unordered_map<std::string, VoidEventHandler> callbacks;

    tda::TDAmeritrade TDAInterface;
    cbp::CoinbasePro CoinbaseProInterface;
    iex::IEXCloud IEXCloudInterface;
    halext::Halext HalextInterface;
    
public:
    Model()=default;
    ~Model()=default;
    
    void addEventHandler(const std::string & key, const VoidEventHandler & handler) {
        this->callbacks[key] = handler;
    }

    tda::TDAmeritrade & getTDAInterface();
    void setTDAInterface(const tda::TDAmeritrade & tda);
};

#endif