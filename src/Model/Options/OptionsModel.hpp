#ifndef OptionsModel_hpp
#define OptionsModel_hpp

#include "../Model.hpp"
#include <cmath>

class OptionsModel: public Model
{
private:
    bool active = false;
    double naiveGammaExposure = 0.0;
    std::string tickerSymbol;
    tda::OptionChain optionChainData;
    std::vector<tda::OptionsDateTimeObj> callOptionArray;
    std::vector<tda::OptionsDateTimeObj> putOptionArray;
    std::vector<tda::OptionsDateTimeObj> optionsDateTimeObj;
    std::vector<const char*> datetime_array;
    std::vector<std::string> datetimeArray;

public:
    bool isActive() const;
    tda::OptionChain & getOptionChainData();

    tda::OptionsDateTimeObj & getCallOptionObj(int index);
    tda::OptionsDateTimeObj & getPutOptionObj(int index);

    tda::OptionsDateTimeObj & getOptionsDateTimeObj(int index);
    std::vector<const char*> & getDateTimeArray();
    std::string getDateTime(int index);
    void fetchOptionChain(const std::string & ticker, const std::string & strikeCount,
                            const std::string & strategy, const std::string & range,
                            const std::string & expMonth, const std::string & optionType);
    
    void calculateGammaExposure();
    double & getGammaExposure();
};

#endif 