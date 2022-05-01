#ifndef IEXCloud_hpp
#define IEXCloud_hpp

#include <vector>
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

        void import_existing_data();

    public:
        IEXCloud();

        std::vector<FundOwnership> getFundOwnership(std::string symbol);

    };
}


#endif