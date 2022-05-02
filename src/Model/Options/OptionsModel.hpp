#ifndef OptionsModel_hpp
#define OptionsModel_hpp

#include "../Model.hpp"
#include "TDA.hpp"
#include <cmath>

class OptionsModel: public Model
{
private:
    bool active = false;
    double naiveGammaExposure = 0.0;
    String tickerSymbol;
    tda::OptionChain optionChainData;
    std::vector<tda::OptionsDateTimeObj> callOptionArray;
    std::vector<tda::OptionsDateTimeObj> putOptionArray;
    std::vector<tda::OptionsDateTimeObj> optionsDateTimeObj;
    std::vector<const char*> datetime_array;
    std::vector<String> datetimeArray;

public:
    bool isActive() const;
    tda::OptionChain & getOptionChainData();

    tda::OptionsDateTimeObj & getCallOptionObj(int index);
    tda::OptionsDateTimeObj & getPutOptionObj(int index);

    tda::OptionsDateTimeObj & getOptionsDateTimeObj(int index);
    std::vector<const char*> & getDateTimeArray();
    String getDateTime(int index);
    void fetchOptionChain(CRString ticker, CRString strikeCount,
                            CRString strategy, CRString range,
                            CRString expMonth, CRString optionType);
    
    void calculateGammaExposure();
    double & getGammaExposure();
};

#endif 