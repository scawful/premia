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

std::vector<const char*> & 
OptionsModel::getDateTimeArray()
{
    return this->datetime_array;
}

std::string 
OptionsModel::getDateTime(int index)
{
    return this->datetimeArray.at(index);
}

void 
OptionsModel::fetchOptionChain(const std::string & ticker, const std::string & strikeCount,
                               const std::string & strategy, const std::string & range,
                               const std::string & expMonth, const std::string & optionType)
{
    optionChainData = getTDAInterface().getOptionChain( ticker, "ALL", strikeCount, true, strategy, "ALL", "ALL", "ALL" );
    optionsDateTimeObj = optionChainData.getOptionsDateTimeObj();
    callOptionArray = optionChainData.getCallOptionArray();
    putOptionArray = optionChainData.getPutOptionArray();
    std::vector<tda::OptionsDateTimeObj> temp_vec = optionsDateTimeObj;
    for ( int i = 0; i < temp_vec.size(); i++) {
        datetime_array.push_back(temp_vec[i].datetime.data());
        datetimeArray.push_back(temp_vec[i].datetime);
    }
    active = true;
}

void 
OptionsModel::calculateGammaExposure()
{
    int i = 1;
    for (const auto & eachOption : optionsDateTimeObj) {
        for (const auto & eachStrike : eachOption.strikePriceObj) {
            double strikeGammaExposure = i * 100;
            double gamma = boost::lexical_cast<double>(eachStrike.raw_option.at("gamma"));
            if (isnan(gamma)) {
                gamma = 0.0;
            }
            strikeGammaExposure *= gamma;
            strikeGammaExposure *= boost::lexical_cast<double>(eachStrike.raw_option.at("openInterest"));
            naiveGammaExposure += strikeGammaExposure;
        }
        i = -1;
    }
}

double & 
OptionsModel::getGammaExposure()
{
    return naiveGammaExposure;
}