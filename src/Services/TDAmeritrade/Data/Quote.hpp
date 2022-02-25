#ifndef Quote_hpp
#define Quote_hpp

#include "../../../core.hpp"

namespace tda 
{
    class Quote 
    {
    private:
        boost::property_tree::ptree quoteData;
        std::map<std::string, std::string> quoteVariables;

        void initVariables();

    public:
        Quote();
        Quote( boost::property_tree::ptree quote_data );

        void updateTree( boost::property_tree::ptree quote_data );

        std::string getQuoteVariable( std::string variable );

        void clear();
    };
}

#endif