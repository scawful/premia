#include "options_model.h"

namespace premia {
bool OptionsModel::isActive() const { return active; }

tda::OptionChain& OptionsModel::getOptionChainData() {
  return this->optionChainData;
}

tda::OptionsDateTimeObj& OptionsModel::getCallOptionObj(int index) {
  return callOptionArray.at(index);
}

tda::OptionsDateTimeObj& OptionsModel::getPutOptionObj(int index) {
  return putOptionArray.at(index);
}

tda::OptionsDateTimeObj& OptionsModel::getOptionsDateTimeObj(int index) {
  return this->optionsDateTimeObj.at(index);
}

std::vector<const char*>& OptionsModel::getDateTimeArray() {
  return this->datetime_array;
}

std::vector<std::string>& OptionsModel::getDateTimeArrayStr() {
  return this->datetimeArray;
}

std::string OptionsModel::getDateTime(int index) {
  return this->datetimeArray.at(index);
}

void OptionsModel::fetchOptionChain(const std::string &ticker, const std::string &strikeCount,
                                    const std::string &strategy, const std::string &range,
                                    const std::string &expMonth, const std::string &optionType) {
  if (!datetimeEpochArray.empty()) {
    datetimeEpochArray.clear();
    datetime_array.clear();
    datetimeArray.clear();
    gammaAtExpiryArray.clear();
  }

  optionChainData = tda::TDA::getInstance().getOptionChain(
      ticker, strikeCount, strategy, "ALL", "ALL", "ALL");
  optionsDateTimeObj = optionChainData.getOptionsDateTimeObj();
  callOptionArray = optionChainData.getCallOptionArray();
  putOptionArray = optionChainData.getPutOptionArray();
  for (const auto& eachOption : callOptionArray) {
    std::tm t = {};
    std::istringstream ss(eachOption.datetime.substr(0, 11));
    if (ss >> std::get_time(&t, "%Y-%m-%d")) {
      datetimeEpochArray.push_back((double)std::mktime(&t));
    } else {
      std::cout << "expiration date parsing failed for " << eachOption.datetime
                << std::endl;
    }
    datetime_array.push_back(eachOption.datetime.data());
    datetimeArray.push_back(eachOption.datetime);
  }
  active = true;
}

void OptionsModel::calculateGammaExposure() {
  for (const auto& eachOption : callOptionArray) {
    auto const& date = eachOption.datetime;
    auto daysTilExpiry = boost::lexical_cast<int>(date.substr(11, date.size()));
    for (const auto& eachStrike : eachOption.strikePriceObj) {
      double strikeGammaExposure = 100;
      double gamma =
          boost::lexical_cast<double>(eachStrike.raw_option.at("gamma"));
      if (std::isnan(gamma)) {
        gamma = 0.0;
      }
      strikeGammaExposure *= gamma;
      strikeGammaExposure *=
          boost::lexical_cast<double>(eachStrike.raw_option.at("openInterest"));
      naiveGammaExposure += strikeGammaExposure;
      callGammaAtExpiryArray.push_back(naiveGammaExposure);
      gammaAtExpiryArray.push_back(naiveGammaExposure);
      double vega =
          boost::lexical_cast<double>(eachStrike.raw_option.at("vega"));
      vegaExposureArray.push_back(vega);

      // vanna
      double stockPrice = boost::lexical_cast<double>(
          optionChainData.getUnderlyingDataVariable("mark"));
      double volatility =
          boost::lexical_cast<double>(eachStrike.raw_option.at("volatility"));
      double strikePrice = boost::lexical_cast<double>(eachStrike.strikePrice);
      double vannaPartOne =
          (log(stockPrice / strikePrice) + (volatility / 2) * daysTilExpiry) /
          (volatility * sqrt(daysTilExpiry));
      double partOneSqr = pow(vannaPartOne, 2) / 2;
      double vannaPartTwo = exp(-partOneSqr) * (1 / (2 * 3.14));
      double vannaPartThree =
          sqrt(daysTilExpiry) * vannaPartTwo * (1 - vannaPartTwo);
      if (vannaPartThree != 0) {
        naiveVannaExposureArray.push_back(vannaPartThree);
      }

      double volgaPartOne = vannaPartOne - (volatility * sqrt(daysTilExpiry));
      double volgaPartTwo = vega * (volgaPartOne / volatility);
      if (volgaPartTwo != 0 && !std::isinf(volgaPartTwo)) {
        volgaExposureArray.push_back(volgaPartTwo);
      }
    }
  }

  for (const auto& eachOption : putOptionArray) {
    int i = 0;
    auto const& date = eachOption.datetime;
    for (const auto& eachStrike : eachOption.strikePriceObj) {
      double strikeGammaExposure = -100;
      double gamma =
          boost::lexical_cast<double>(eachStrike.raw_option.at("gamma"));
      if (std::isnan(gamma)) {
        gamma = 0.0;
      }
      strikeGammaExposure *= gamma;
      strikeGammaExposure *=
          boost::lexical_cast<double>(eachStrike.raw_option.at("openInterest"));
      naiveGammaExposure += strikeGammaExposure;
      putGammaAtExpiryArray.push_back(naiveGammaExposure);
      gammaAtExpiryArray[i] += naiveGammaExposure;
      double vol =
          boost::lexical_cast<double>(eachStrike.raw_option.at("vega"));
      vegaExposureArray[i] = (vol + vegaExposureArray[i]) / 2;
      i++;
    }
  }
}

double& OptionsModel::getGammaExposure() { return naiveGammaExposure; }

double& OptionsModel::getGammaAtExpiry(int i) {
  return gammaAtExpiryArray.at(i);
}

std::vector<double>& OptionsModel::getGammaAtExpiryList() {
  return gammaAtExpiryArray;
}

std::vector<double>& OptionsModel::getCallGammaAtExpiryList() {
  return callGammaAtExpiryArray;
}

std::vector<double>& OptionsModel::getPutGammaAtExpiryList() {
  return putGammaAtExpiryArray;
}

std::vector<double>& OptionsModel::getNaiveVannaExposureList() {
  return naiveVannaExposureArray;
}

std::vector<double>& OptionsModel::getDatetimeEpochArray() {
  return datetimeEpochArray;
}

std::vector<double>& OptionsModel::getVegaExposureArray() {
  return vegaExposureArray;
}

std::vector<double>& OptionsModel::getVolgaExposureArray() {
  return volgaExposureArray;
}
}  // namespace premia