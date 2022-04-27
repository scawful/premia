#ifndef OptionsModel_hpp
#define OptionsModel_hpp

#include "../Model.hpp"

class OptionsModel: public Model
{
private:
    bool active = false;
    std::string tickerSymbol;
    tda::OptionChain optionChainData;
    std::vector<tda::OptionsDateTimeObj> optionsDateTimeObj;
    std::vector<const char*> datetime_array;

public:
    bool isActive() const;
    tda::OptionChain & getOptionChainData();
    tda::OptionsDateTimeObj & getOptionsDateTimeObj(int index);
    std::vector<const char*> & getDateTimeArray();
    void fetchOptionChain(const std::string & ticker, const std::string & contractType, const std::string & strikeCount,
                            bool includeQuotes, const std::string & strategy, const std::string & range,
                            const std::string & expMonth, const std::string & optionType);
};

#endif 