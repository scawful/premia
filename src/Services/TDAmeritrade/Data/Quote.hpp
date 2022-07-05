#ifndef Quote_hpp
#define Quote_hpp

#include <boost/property_tree/ptree.hpp>
#include "Premia.hpp"

namespace premia {
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
}  // namespace premia
#endif