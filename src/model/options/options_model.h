#ifndef OptionsModel_hpp
#define OptionsModel_hpp

#include <cmath>
#include <string>
#include <vector>

#include "metatypes.h"
#include "model/model.h"
#include "singletons/TDA.hpp"


namespace premia {
class OptionsModel : public Model {
 private:
  bool active = false;
  double naiveGammaExposure = 0.0;
  std::string tickerSymbol;
  tda::OptionChain optionChainData;
  std::vector<tda::OptionsDateTimeObj> callOptionArray;
  std::vector<tda::OptionsDateTimeObj> putOptionArray;
  std::vector<tda::OptionsDateTimeObj> optionsDateTimeObj;
  std::vector<const char*> datetime_array;

  std::vector<double> gammaAtExpiryArray;
  std::vector<double> callGammaAtExpiryArray;
  std::vector<double> putGammaAtExpiryArray;

  std::vector<std::string> datetimeArray;
  std::vector<double> datetimeEpochArray;

  std::vector<double> naiveVannaExposureArray;
  std::vector<double> volgaExposureArray;

  std::vector<double> vegaExposureArray;

 public:
  bool isActive() const;
  tda::OptionChain& getOptionChainData();

  tda::OptionsDateTimeObj& getCallOptionObj(int index);
  tda::OptionsDateTimeObj& getPutOptionObj(int index);
  tda::OptionsDateTimeObj& getOptionsDateTimeObj(int index);
  std::vector<const char*>& getDateTimeArray();
  std::vector<std::string>& getDateTimeArrayStr();
  std::string getDateTime(int index);
  void fetchOptionChain(const std::string& ticker,
                        const std::string& strikeCount,
                        const std::string& strategy, const std::string& range,
                        const std::string& expMonth,
                        const std::string& optionType);
  void calculateGammaExposure();
  double& getGammaExposure();
  double& getGammaAtExpiry(int i);
  std::vector<double>& getGammaAtExpiryList();
  std::vector<double>& getCallGammaAtExpiryList();
  std::vector<double>& getPutGammaAtExpiryList();

  std::vector<double>& getNaiveVannaExposureList();

  std::vector<double>& getDatetimeEpochArray();

  std::vector<double>& getVegaExposureArray();

  std::vector<double>& getVolgaExposureArray();
};
}  // namespace premia

#endif