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

    public:
        Quote();

        void setQuoteVariable(std::string key, std::string value);
        std::string getQuoteVariable(std::string variable);

        void clear();
    };
}

#endif