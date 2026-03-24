#include "options_model.h"

#include <iomanip>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include "premia/core/application/composition_root.hpp"

namespace premia {
bool OptionsModel::isActive() const { return active; }

const core::application::OptionChainSnapshot& OptionsModel::getCoreSnapshot() const {
  return core_snapshot;
}

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
  datetimeEpochArray.clear();
  datetime_array.clear();
  datetimeArray.clear();
  gammaAtExpiryArray.clear();
  callGammaAtExpiryArray.clear();
  putGammaAtExpiryArray.clear();
  naiveVannaExposureArray.clear();
  volgaExposureArray.clear();
  vegaExposureArray.clear();
  naiveGammaExposure = 0.0;

  const auto snapshot = core::application::CompositionRoot::Instance()
                            .Options()
                            .GetOptionChainSnapshot(ticker, strikeCount, strategy,
                                                    range, expMonth, optionType);
  loadFromCoreSnapshot(snapshot);
  active = true;
}

void OptionsModel::calculateGammaExposure() {
  naiveGammaExposure = 0.0;
  gammaAtExpiryArray.clear();
  callGammaAtExpiryArray.clear();
  putGammaAtExpiryArray.clear();
  naiveVannaExposureArray.clear();
  volgaExposureArray.clear();
  vegaExposureArray.clear();

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

auto OptionsModel::makeRawOptionMap(const std::map<std::string, std::string>& values)
    -> std::unordered_map<std::string, std::string> {
  return std::unordered_map<std::string, std::string>(values.begin(), values.end());
}

void OptionsModel::loadFromCoreSnapshot(
    const core::application::OptionChainSnapshot& snapshot) {
  core_snapshot = snapshot;
  optionChainData = tda::OptionChain();
  optionChainData.setOptionChainVariable("symbol", snapshot.symbol);
  optionChainData.setOptionChainVariable("volatility", snapshot.volatility);
  optionChainData.setUnderlyingVariable("description", snapshot.description);
  optionChainData.setUnderlyingVariable("bid", snapshot.bid);
  optionChainData.setUnderlyingVariable("ask", snapshot.ask);
  optionChainData.setUnderlyingVariable("openPrice", snapshot.open_price);
  optionChainData.setUnderlyingVariable("close", snapshot.close_price);
  optionChainData.setUnderlyingVariable("highPrice", snapshot.high_price);
  optionChainData.setUnderlyingVariable("lowPrice", snapshot.low_price);
  optionChainData.setUnderlyingVariable("totalVolume", snapshot.total_volume);
  optionChainData.setUnderlyingVariable("markPercentChange", "N/A");
  optionChainData.setUnderlyingVariable("exchangeName", "N/A");
  optionChainData.setUnderlyingVariable("mark", snapshot.bid);

  callOptionArray.clear();
  putOptionArray.clear();
  optionsDateTimeObj.clear();

  for (const auto& expiration : snapshot.expirations) {
    tda::OptionsDateTimeObj call_object;
    tda::OptionsDateTimeObj put_object;
    call_object.datetime = expiration.label;
    put_object.datetime = expiration.label;

    for (const auto& row : expiration.rows) {
      tda::StrikePriceMap call_strike;
      call_strike.strikePrice = row.strike;
      call_strike.raw_option = makeRawOptionMap({
          {"symbol", row.call_symbol},
          {"strikePrice", row.strike},
          {"bid", row.call_bid},
          {"ask", row.call_ask},
          {"last", row.call_last},
          {"netChange", row.call_change},
          {"delta", row.call_delta},
          {"gamma", row.call_gamma},
          {"theta", row.call_theta},
          {"vega", row.call_vega},
          {"openInterest", row.call_open_interest},
          {"volatility", snapshot.volatility},
      });
      call_object.strikePriceObj.push_back(call_strike);

      tda::StrikePriceMap put_strike;
      put_strike.strikePrice = row.strike;
      put_strike.raw_option = makeRawOptionMap({
          {"symbol", row.put_symbol},
          {"strikePrice", row.strike},
          {"bid", row.put_bid},
          {"ask", row.put_ask},
          {"last", row.put_last},
          {"netChange", row.put_change},
          {"delta", row.put_delta},
          {"gamma", row.put_gamma},
          {"theta", row.put_theta},
          {"vega", row.put_vega},
          {"openInterest", row.put_open_interest},
          {"volatility", snapshot.volatility},
      });
      put_object.strikePriceObj.push_back(put_strike);
    }

    optionChainData.addOptionsDateTimeObj(call_object);
    optionChainData.addOptionsDateTimeObj(call_object, 1);
    optionChainData.addOptionsDateTimeObj(put_object, 0);
    optionsDateTimeObj.push_back(call_object);
    callOptionArray.push_back(call_object);
    putOptionArray.push_back(put_object);

    std::tm t = {};
    const auto date_token = expiration.label.substr(0, 10);
    std::istringstream ss(date_token);
    if (ss >> std::get_time(&t, "%Y-%m-%d")) {
      datetimeEpochArray.push_back((double)std::mktime(&t));
    } else {
      datetimeEpochArray.push_back(0.0);
    }
    datetimeArray.push_back(expiration.label);
  }

  datetime_array.reserve(datetimeArray.size());
  for (auto& value : datetimeArray) {
    datetime_array.push_back(value.c_str());
  }
}
}  // namespace premia
