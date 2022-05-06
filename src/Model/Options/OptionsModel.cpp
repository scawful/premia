#include "OptionsModel.hpp"


bool 
OptionsModel::isActive() const {
    return active;
}

tda::OptionChain& 
OptionsModel::getOptionChainData() {
    return this->optionChainData;
}

tda::OptionsDateTimeObj & 
OptionsModel::getCallOptionObj(int index) {
    return callOptionArray.at(index);
}

tda::OptionsDateTimeObj & 
OptionsModel::getPutOptionObj(int index) {
    return putOptionArray.at(index);
}

tda::OptionsDateTimeObj & 
OptionsModel::getOptionsDateTimeObj(int index) {
    return this->optionsDateTimeObj.at(index);    
}

ArrayList<const char*> & 
OptionsModel::getDateTimeArray() {
    return this->datetime_array;
}

ArrayList<String> & 
OptionsModel::getDateTimeArrayStr() {
    return this->datetimeArray;
}

String 
OptionsModel::getDateTime(int index) {
    return this->datetimeArray.at(index);
}

void 
OptionsModel::fetchOptionChain(CRString ticker, CRString strikeCount,
                               CRString strategy, CRString range,
                               CRString expMonth, CRString optionType) {
    if (!datetimeEpochArray.empty()) {
        datetimeEpochArray.clear();
        datetime_array.clear();
        datetimeArray.clear();
        gammaAtExpiryArray.clear();
    }

    optionChainData = tda::TDA::getInstance().getOptionChain(ticker, strikeCount, strategy, "ALL", "ALL", "ALL");
    optionsDateTimeObj = optionChainData.getOptionsDateTimeObj();
    callOptionArray = optionChainData.getCallOptionArray();
    putOptionArray = optionChainData.getPutOptionArray();
    for (const auto & eachOption : callOptionArray) {
        std::tm t = {};
        std::istringstream ss(eachOption.datetime.substr(0,11));
        if (ss >> std::get_time(&t, "%Y-%m-%d")) {
            datetimeEpochArray.push_back(std::mktime(&t));
        } else {
            std::cout << "expiration date parsing failed for " << eachOption.datetime << std::endl;
        }
        datetime_array.push_back(eachOption.datetime.data());
        datetimeArray.push_back(eachOption.datetime);
    }
    active = true;
}

void 
OptionsModel::calculateGammaExposure() {
    for (const auto & eachOption : callOptionArray) {
        auto date = eachOption.datetime;
        auto daysTilExpiry = boost::lexical_cast<int>(date.substr(11,date.size()));
        for (const auto & eachStrike : eachOption.strikePriceObj) {
            double strikeGammaExposure = 100;
            double gamma = boost::lexical_cast<double>(eachStrike.raw_option.at("gamma"));
            if (isnan(gamma)) {
                gamma = 0.0;
            }
            strikeGammaExposure *= gamma;
            strikeGammaExposure *= boost::lexical_cast<double>(eachStrike.raw_option.at("openInterest"));
            naiveGammaExposure += strikeGammaExposure;
            callGammaAtExpiryArray.push_back(naiveGammaExposure);
            gammaAtExpiryArray.push_back(naiveGammaExposure);
            vegaExposureArray.push_back(boost::lexical_cast<double>(eachStrike.raw_option.at("vega")));

            // vanna 
            double high = boost::lexical_cast<double>(optionChainData.getUnderlyingDataVariable("highPrice"));
            double low = boost::lexical_cast<double>(optionChainData.getUnderlyingDataVariable("lowPrice"));
            double stockPrice = (high + low) / 2;
            std::cout << stockPrice << std::endl;
            double volatility =  boost::lexical_cast<double>(eachStrike.raw_option.at("volatility"));
            std::cout << volatility << std::endl;
            double strikePrice = boost::lexical_cast<double>(eachStrike.strikePrice);
            std::cout << strikePrice << std::endl;
            double vannaPartOne = (log(stockPrice / strikePrice) + (volatility / 2) * daysTilExpiry) / (volatility * sqrt(daysTilExpiry));
            std::cout << vannaPartOne << std::endl;
            double vannaPartTwo = exp(-((pow(vannaPartOne, 2)/2))) * (1/2 * M_1_PI);
            std::cout << vannaPartTwo << std::endl;
            double vannaPartThree = sqrt(daysTilExpiry) * vannaPartTwo * (1 - vannaPartTwo);
            std::cout << vannaPartThree << std::endl;
            naiveVannaExposureArray.push_back(vannaPartThree);
            // callVannaExposureArray.push_back(vannaPartThree); 
        }
    }

    for (const auto & eachOption : putOptionArray) {
        int i = 0;
        auto date = eachOption.datetime;
        auto daysTilExpiry = boost::lexical_cast<int>(date.substr(11,date.size()));
        for (const auto & eachStrike : eachOption.strikePriceObj) {
            double strikeGammaExposure = -100;
            double gamma = boost::lexical_cast<double>(eachStrike.raw_option.at("gamma"));
            if (isnan(gamma)) {
                gamma = 0.0;
            }
            strikeGammaExposure *= gamma;
            strikeGammaExposure *= boost::lexical_cast<double>(eachStrike.raw_option.at("openInterest"));
            naiveGammaExposure += strikeGammaExposure;
            putGammaAtExpiryArray.push_back(naiveGammaExposure);
            gammaAtExpiryArray[i] += naiveGammaExposure;
            double vol = boost::lexical_cast<double>(eachStrike.raw_option.at("vega"));
            vegaExposureArray[i] = (vol + vegaExposureArray[i]) / 2;
            double high = boost::lexical_cast<double>(optionChainData.getUnderlyingDataVariable("highPrice"));
            double low = boost::lexical_cast<double>(optionChainData.getUnderlyingDataVariable("lowPrice"));
            double stockPrice = (high + low) / 2;
            double volatility =  boost::lexical_cast<double>(eachStrike.raw_option.at("volatility"));
            double strikePrice = boost::lexical_cast<double>(eachStrike.strikePrice);
            double vannaPartOne = (log(stockPrice / strikePrice) + (volatility / 2) * daysTilExpiry) / (volatility * sqrt(daysTilExpiry));
            double vannaPartTwo = pow(M_E, (-((pow(vannaPartOne, 2)/2)))) * (1/2 * M_1_PI);
            double vannaPartThree = sqrt(daysTilExpiry) * vannaPartTwo * (1 - vannaPartTwo);
            naiveVannaExposureArray[i] += vannaPartThree;
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

ArrayList<double> & 
OptionsModel::getGammaAtExpiryList() {
    return gammaAtExpiryArray;
}

ArrayList<double> & 
OptionsModel::getCallGammaAtExpiryList() {
    return callGammaAtExpiryArray;
}

ArrayList<double> & 
OptionsModel::getPutGammaAtExpiryList() {
    return putGammaAtExpiryArray;
}

ArrayList<double> & 
OptionsModel::getNaiveVannaExposureList() {
    return naiveVannaExposureArray;
}

ArrayList<double> & 
OptionsModel::getCallVannaExposureList() {
    return callVannaExposureArray;
}

ArrayList<double> & 
OptionsModel::getPutVannaExposureList() {
    return putVannaExposureArray;
}

ArrayList<double> & 
OptionsModel::getDatetimeEpochArray() {
    return datetimeEpochArray;
}

ArrayList<double> & 
OptionsModel::getVegaExposureArray() {
    return vegaExposureArray;
}