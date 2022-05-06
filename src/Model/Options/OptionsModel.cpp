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
        auto daysTilExpiry = stoi(date.substr(11,date.size()));
        for (const auto & eachStrike : eachOption.strikePriceObj) {
            double strikeGammaExposure = 100;
            double gamma = boost::lexical_cast<double>(eachStrike.raw_option.at("gamma"));
            if (isnan(gamma)) {
                naiveGammaExposure = callGammaAtExpiryArray.at(callGammaAtExpiryArray.size() - 1);
            } else {
                strikeGammaExposure *= gamma;
                strikeGammaExposure *= boost::lexical_cast<double>(eachStrike.raw_option.at("openInterest"));
                naiveGammaExposure += strikeGammaExposure;
            }
            callGammaAtExpiryArray.push_back(naiveGammaExposure);
            gammaAtExpiryArray.push_back(naiveGammaExposure);
            vegaExposureArray.push_back(boost::lexical_cast<double>(eachStrike.raw_option.at("vega")));

            // vanna 
            // double stockPrice = stod(optionChainData.getUnderlyingDataVariable("lastPrice"));
            // double volatility = stod(optionChainData.getUnderlyingDataVariable("volatility"));
            // double strikePrice = stod(eachStrike.strikePrice);
            // double vannaPartOne = (log(stockPrice / strikePrice) + (volatility / 2) * daysTilExpiry) / (volatility * sqrt(daysTilExpiry));
            // double vannaPartTwo = pow(M_E, (-((pow(vannaPartOne, 2)/2)))) * (1/2 * M_1_PI);
            // double vannaPartThree = sqrt(daysTilExpiry) * vannaPartTwo * (1 - vannaPartTwo);
            // naiveVannaExposureArray.push_back(vannaPartThree);
            // callVannaExposureArray.push_back(vannaPartThree); 
        }
    }

    for (const auto & eachOption : putOptionArray) {
        int i = 0;
        for (const auto & eachStrike : eachOption.strikePriceObj) {
            double strikeGammaExposure = -100;
            double gamma = boost::lexical_cast<double>(eachStrike.raw_option.at("gamma"));
            if (isnan(gamma)) {
                naiveGammaExposure = putGammaAtExpiryArray.at(putGammaAtExpiryArray.size() - 1);
            } else {
                strikeGammaExposure *= gamma;
                strikeGammaExposure *= boost::lexical_cast<double>(eachStrike.raw_option.at("openInterest"));
                naiveGammaExposure += strikeGammaExposure;
            }
            putGammaAtExpiryArray.push_back(naiveGammaExposure);
            gammaAtExpiryArray[i] += naiveGammaExposure;
            double vol = boost::lexical_cast<double>(eachStrike.raw_option.at("vega"));
            vegaExposureArray[i] = (vol + vegaExposureArray[i]) / 2;
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