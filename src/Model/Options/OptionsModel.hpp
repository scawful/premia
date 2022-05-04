#ifndef OptionsModel_hpp
#define OptionsModel_hpp

#include "../Model.hpp"
#include "Interface/TDA.hpp"
#include <cmath>

class OptionsModel: public Model
{
private:
    bool active = false;
    double naiveGammaExposure = 0.0;
    String tickerSymbol;
    tda::OptionChain optionChainData;
    ArrayList<tda::OptionsDateTimeObj> callOptionArray;
    ArrayList<tda::OptionsDateTimeObj> putOptionArray;
    ArrayList<tda::OptionsDateTimeObj> optionsDateTimeObj;
    ArrayList<const char*> datetime_array;
    ArrayList<String> datetimeArray;

public:
    bool isActive() const;
    tda::OptionChain & getOptionChainData();

    tda::OptionsDateTimeObj & getCallOptionObj(int index);
    tda::OptionsDateTimeObj & getPutOptionObj(int index);

    tda::OptionsDateTimeObj & getOptionsDateTimeObj(int index);
    ArrayList<const char*> & getDateTimeArray();
    String getDateTime(int index);
    void fetchOptionChain(CRString ticker, CRString strikeCount,
                            CRString strategy, CRString range,
                            CRString expMonth, CRString optionType);
    
    void calculateGammaExposure();
    double & getGammaExposure();
};

#endif 