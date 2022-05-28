#include "Quote.hpp"
namespace Premia {
using namespace tda;

Quote::Quote() = default;

void Quote::setQuoteVariable(CRString key, CRString value) {
  quoteVariables[key] = value;
}

String Quote::getQuoteVariable(CRString variable) {
  return quoteVariables[variable];
}

void Quote::clear() { quoteVariables.clear(); }
}  // namespace Premia