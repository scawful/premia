#ifndef IEX_FundOwnership_hpp
#define IEX_FundOwnership_hpp

#include "../../../core.hpp"

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
        void setReportedMarketValue(const std::string & reportedMarketValue) { reported_market_value = reportedMarketValue; }

        std::string reportedHolding() const { return reported_holding; }
        void setReportedHolding(const std::string & reportedHolding) { reported_holding = reportedHolding; }

        time_t reportDate() const { return report_date; }
        void setReportDate(const time_t &reportDate) { report_date = reportDate; }

        std::string entityProperName() const { return entity_proper_name; }
        void setEntityProperName(const std::string &entityProperName) { entity_proper_name = entityProperName; }

        std::string adjustedMarketValue() const { return adjusted_market_value; }
        void setAdjustedMarketValue(const std::string & adjustedMarketValue) { adjusted_market_value = adjustedMarketValue; }

        std::string getSymbol() const { return symbol; }
        void setSymbol(const std::string & symbol_) { symbol = symbol_; }

        std::string adjustedHoldings() const { return adjusted_holdings; }
        void setAdjustedHoldings(const std::string & adjustedHoldings) { adjusted_holdings = adjustedHoldings; }
    };
}

#endif 