#ifndef Quote_hpp
#define Quote_hpp

#include "Premia.hpp"
#include "Library/Boost.hpp"

namespace tda 
{
    class Quote 
    {
    private:
        boost::property_tree::ptree quoteData;
        StringMap quoteVariables;

    public:
        Quote();

        void setQuoteVariable(CRString key, CRString value);
        String getQuoteVariable(CRString variable);

        void clear();
    };
}

#endif