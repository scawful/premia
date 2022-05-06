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

    ArrayList<double> gammaAtExpiryArray;
    ArrayList<double> callGammaAtExpiryArray;
    ArrayList<double> putGammaAtExpiryArray;

    ArrayList<String> datetimeArray;
    ArrayList<double> datetimeEpochArray;

    ArrayList<double> naiveVannaExposureArray;
    ArrayList<double> volgaExposureArray;

    ArrayList<double> vegaExposureArray;

public:
    bool isActive() const;
    tda::OptionChain & getOptionChainData();

    tda::OptionsDateTimeObj & getCallOptionObj(int index);
    tda::OptionsDateTimeObj & getPutOptionObj(int index);
    tda::OptionsDateTimeObj & getOptionsDateTimeObj(int index);
    ArrayList<const char*> & getDateTimeArray();
    ArrayList<String> & getDateTimeArrayStr();
    String getDateTime(int index);
    void fetchOptionChain(CRString ticker, CRString strikeCount,
                          CRString strategy, CRString range,
                          CRString expMonth, CRString optionType);
    void calculateGammaExposure();
    double & getGammaExposure();
    double & getGammaAtExpiry(int i);
    ArrayList<double> & getGammaAtExpiryList();
    ArrayList<double> & getCallGammaAtExpiryList();
    ArrayList<double> & getPutGammaAtExpiryList();

    ArrayList<double> & getNaiveVannaExposureList();

    ArrayList<double> & getDatetimeEpochArray();

    ArrayList<double> & getVegaExposureArray();

    ArrayList<double> & getVolgaExposureArray();
};

#endif 