#ifndef IEX_FundOwnership_hpp
#define IEX_FundOwnership_hpp

#include "Premia.hpp"
namespace Premia {
namespace iex {
class FundOwnership {
 private:
  time_t report_date;
  String symbol;
  String entity_proper_name;
  String adjusted_holdings;
  String adjusted_market_value;
  String reported_holding;
  String reported_market_value;

 public:
  FundOwnership() = default;

  String reportedMarketValue() const { return reported_market_value; }
  void setReportedMarketValue(CRString reportedMarketValue) {
    reported_market_value = reportedMarketValue;
  }

  String reportedHolding() const { return reported_holding; }
  void setReportedHolding(CRString reportedHolding) {
    reported_holding = reportedHolding;
  }

  time_t reportDate() const { return report_date; }
  void setReportDate(const time_t &reportDate) { report_date = reportDate; }

  String entityProperName() const { return entity_proper_name; }
  void setEntityProperName(CRString entityProperName) {
    entity_proper_name = entityProperName;
  }

  String adjustedMarketValue() const { return adjusted_market_value; }
  void setAdjustedMarketValue(CRString adjustedMarketValue) {
    adjusted_market_value = adjustedMarketValue;
  }

  String getSymbol() const { return symbol; }
  void setSymbol(CRString symbol_) { symbol = symbol_; }

  String adjustedHoldings() const { return adjusted_holdings; }
  void setAdjustedHoldings(CRString adjustedHoldings) {
    adjusted_holdings = adjustedHoldings;
  }
};
}  // namespace iex
}  // namespace Premia
#endif