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
public:
    Model()=default;
    Model(const Model &)=default;
    virtual ~Model()=default; 
};

#endif