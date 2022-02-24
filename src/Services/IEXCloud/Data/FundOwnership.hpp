#ifndef IEX_FundOwnership_hpp
#define IEX_FundOwnership_hpp

#include "../../../core.hpp"

namespace iex
{
    // "symbol": "AAPL",
    // "id": "MUTUAL_FUND_HOLDERS",
    // "adjHolding": 177998597,
    // "adjMv": 20036225482,
    // "entityProperName": "PARNASSUS INCOME FUNDS",
    // "report_date": 1676849549242,
    // "reportedHolding": 174277099,
    // "reportedMv": 19808288329,
    // "updated": 1615663857444
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
        FundOwnership() { }

        std::string reportedMarketValue() const { return reported_market_value; }
        void setReportedMarketValue(std::string reportedMarketValue) { reported_market_value = reportedMarketValue; }

        std::string reportedHolding() const { return reported_holding; }
        void setReportedHolding(std::string reportedHolding) { reported_holding = reportedHolding; }

        time_t reportDate() const { return report_date; }
        void setReportDate(const time_t &reportDate) { report_date = reportDate; }

        std::string entityProperName() const { return entity_proper_name; }
        void setEntityProperName(const std::string &entityProperName) { entity_proper_name = entityProperName; }

        std::string adjustedMarketValue() const { return adjusted_market_value; }
        void setAdjustedMarketValue(std::string adjustedMarketValue) { adjusted_market_value = adjustedMarketValue; }

        std::string getSymbol() const { return symbol; }
        void setSymbol(const std::string &symbol_) { symbol = symbol_; }

        std::string adjustedHoldings() const { return adjusted_holdings; }
        void setAdjustedHoldings(std::string adjustedHoldings) { adjusted_holdings = adjustedHoldings; }
    };
}

#endif 