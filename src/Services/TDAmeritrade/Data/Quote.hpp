#ifndef Quote_hpp
#define Quote_hpp

#include "Library/Boost.hpp"
#include "Premia.hpp"
namespace Premia {
namespace tda {
class Quote {
 private:
  boost::property_tree::ptree quoteData;
  StringMap quoteVariables;

 public:
  Quote();

  void setQuoteVariable(CRString key, CRString value);
  String getQuoteVariable(CRString variable);

  void clear();
};
}  // namespace tda
}  // namespace Premia
#endif