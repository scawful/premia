#ifndef IEX_FundOwnership_hpp
#define IEX_FundOwnership_hpp

#include "Premia.hpp"

namespace iex
{
    class FundOwnership
    {
    private:
        time_t report_date;
        std::string symbol;
        std::string entity_proper_name;
        std::string adjusted_holdings;
        std::string adjusted_market_value;
        std::string reported_holding;
        std::string reported_market_value;

    public:
        FundOwnership()=default;

        std::string reportedMarketValue() const { return reported_market_value; }
        void setReportedMarketValue(String reportedMarketValue) { reported_market_value = reportedMarketValue; }

        std::string reportedHolding() const { return reported_holding; }
        void setReportedHolding(String reportedHolding) { reported_holding = reportedHolding; }

        time_t reportDate() const { return report_date; }
        void setReportDate(const time_t &reportDate) { report_date = reportDate; }

        std::string entityProperName() const { return entity_proper_name; }
        void setEntityProperName(String entityProperName) { entity_proper_name = entityProperName; }

        std::string adjustedMarketValue() const { return adjusted_market_value; }
        void setAdjustedMarketValue(String adjustedMarketValue) { adjusted_market_value = adjustedMarketValue; }

        std::string getSymbol() const { return symbol; }
        void setSymbol(String symbol_) { symbol = symbol_; }

        std::string adjustedHoldings() const { return adjusted_holdings; }
        void setAdjustedHoldings(String adjustedHoldings) { adjusted_holdings = adjustedHoldings; }
    };
}

#endif 