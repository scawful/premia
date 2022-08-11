#ifndef OptionChain_hpp
#define OptionChain_hpp

#include "PricingStructures.hpp"
namespace premia {
namespace tda {
class OptionChain {
 private:
  std::unordered_map<std::string, std::string> callExpDateMap;
  std::unordered_map<std::string, std::string> putExpDateMap;
  std::unordered_map<std::string, std::string> optionChainMap;
  std::unordered_map<std::string, std::string> underlyingMap;
  std::vector<OptionsDateTimeObj> callOptionArray;
  std::vector<OptionsDateTimeObj> putOptionArray;
  std::vector<OptionsDateTimeObj> optionsDateTimeVector;

 public:
  OptionChain();

  std::vector<OptionsDateTimeObj> getCallOptionArray() const;
  std::vector<OptionsDateTimeObj> getPutOptionArray() const;
  std::vector<OptionsDateTimeObj> getOptionsDateTimeObj() const;
  std::string getCallVariable(const std::string &variable);
  std::string getPutVariable(const std::string &variable);
  std::string getOptionChainDataVariable(const std::string &variable);
  std::string getUnderlyingDataVariable(const std::string &variable);

  void addOptionsDateTimeObj(const OptionsDateTimeObj& obj);
  void addOptionsDateTimeObj(const OptionsDateTimeObj& obj, int idx);
  void setCallVariable(const std::string &key, const std::string &value);
  void setPutVariable(const std::string &key, const std::string &value);
  void setOptionChainVariable(const std::string &key, const std::string &value);
  void setUnderlyingVariable(const std::string &key, const std::string &value);
};
}  // namespace tda
}  // namespace premia
#endif