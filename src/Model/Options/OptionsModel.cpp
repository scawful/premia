#include "OptionsModel.hpp"


bool OptionsModel::isActive() const
{
    return active;
}

tda::OptionChain& 
OptionsModel::getOptionChainData()
{
    return this->optionChainData;
}

tda::OptionsDateTimeObj & 
OptionsModel::getCallOptionObj(int index)
{
    return callOptionArray.at(index);
}

tda::OptionsDateTimeObj & 
OptionsModel::getPutOptionObj(int index)
{
    return putOptionArray.at(index);
}

tda::OptionsDateTimeObj & 
OptionsModel::getOptionsDateTimeObj(int index)
{
    return this->optionsDateTimeObj.at(index);    
}

ArrayList<const char*> & 
OptionsModel::getDateTimeArray()
{
    return this->datetime_array;
}

String 
OptionsModel::getDateTime(int index)
{
    return this->datetimeArray.at(index);
}

void 
OptionsModel::fetchOptionChain(CRString ticker, CRString strikeCount,
                               CRString strategy, CRString range,
                               CRString expMonth, CRString optionType)
{
    optionChainData = tda::TDA::getInstance().getOptionChain(ticker, strikeCount, strategy, "ALL", "ALL", "ALL");
    optionsDateTimeObj = optionChainData.getOptionsDateTimeObj();
    callOptionArray = optionChainData.getCallOptionArray();
    putOptionArray = optionChainData.getPutOptionArray();
    ArrayList<tda::OptionsDateTimeObj> temp_vec = callOptionArray;
    for ( int i = 0; i < temp_vec.size(); i++) {
        datetime_array.push_back(temp_vec[i].datetime.data());
        datetimeArray.push_back(temp_vec[i].datetime);
    }
    active = true;
}

void 
OptionsModel::calculateGammaExposure()
{
    for (const auto & eachOption : callOptionArray) {
        for (const auto & eachStrike : eachOption.strikePriceObj) {
            double strikeGammaExposure = 100;
            double gamma = boost::lexical_cast<double>(eachStrike.raw_option.at("gamma"));
            if (isnan(gamma)) {
                gamma = 0.0;
            }
            strikeGammaExposure *= gamma;
            strikeGammaExposure *= boost::lexical_cast<double>(eachStrike.raw_option.at("openInterest"));
            naiveGammaExposure += strikeGammaExposure;
            gammaAtExpiryArray.push_back(naiveGammaExposure);
        }
    }

    for (const auto & eachOption : putOptionArray) {
        int i = 0;
        for (const auto & eachStrike : eachOption.strikePriceObj) {
            double strikeGammaExposure = -100;
            double gamma = boost::lexical_cast<double>(eachStrike.raw_option.at("gamma"));
            if (isnan(gamma)) {
                gamma = 0.0;
            }
            strikeGammaExposure *= gamma;
            strikeGammaExposure *= boost::lexical_cast<double>(eachStrike.raw_option.at("openInterest"));
            naiveGammaExposure += strikeGammaExposure;
            gammaAtExpiryArray[i] += naiveGammaExposure;
            i++;
        }
    }
}

double & 
OptionsModel::getGammaExposure() {
    return naiveGammaExposure;
}

double & 
OptionsModel::getGammaAtExpiry(int i) {
    return gammaAtExpiryArray.at(i);
}