#ifndef Quote_hpp
#define Quote_hpp

#include "Premia.hpp"
#include "Boost.hpp"

namespace tda 
{
    class Quote 
    {
    private:
        boost::property_tree::ptree quoteData;
        std::unordered_map<std::string, std::string> quoteVariables;

    public:
        Quote();

        void setQuoteVariable(String key, String value);
        std::string getQuoteVariable(String variable);

        void clear();
    };
}

#endif