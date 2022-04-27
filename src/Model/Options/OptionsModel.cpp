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
OptionsModel::getOptionsDateTimeObj(int index)
{
    return this->optionsDateTimeObj.at(index);    
}

std::vector<const char*> & 
OptionsModel::getDateTimeArray()
{
    return this->datetime_array;
}

void 
OptionsModel::fetchOptionChain(const std::string & ticker, const std::string & contractType, const std::string & strikeCount,
                               bool includeQuotes, const std::string & strategy, const std::string & range,
                               const std::string & expMonth, const std::string & optionType)
{
    optionChainData = getTDAInterface().getOptionChain( ticker, "ALL", "50", true, "SINGLE", "ALL", "ALL", "ALL" );
    optionsDateTimeObj = optionChainData.getOptionsDateTimeObj();
    std::vector<tda::OptionsDateTimeObj> temp_vec = optionsDateTimeObj;
    for ( int i = 0; i < temp_vec.size(); i++) 
    {
        datetime_array.push_back(temp_vec[i].datetime.data());
    }
    active = true;
}
