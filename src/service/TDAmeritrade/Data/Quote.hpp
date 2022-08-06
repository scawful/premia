#ifndef Quote_hpp
#define Quote_hpp

#include <boost/property_tree/ptree.hpp>

#include "premia.h"

namespace premia {
namespace tda {

class Quote {
 private:
  boost::property_tree::ptree quoteData;
  std::unordered_map<std::string, std::string> quoteVariables;

 public:
  Quote();

  void setQuoteVariable(const std::string &key, const std::string &value);
  std::string getQuoteVariable(const std::string &variable);

  void clear();
};

}  // namespace tda
}  // namespace premia
#endif