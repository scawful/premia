#ifndef IEXCloud_hpp
#define IEXCloud_hpp

#include "../core.hpp"
#include "IEXCloud/Client.hpp"
#include "IEXCloud/Parser.hpp"
#include "IEXCloud/Data/FundOwnership.hpp"

namespace iex 
{
    class IEXCloud
    {
    private:
        Client client;
        Parser parser;


    public:
        IEXCloud();

        std::vector<FundOwnership> getFundOwnership(std::string symbol);

    };
}


#endif