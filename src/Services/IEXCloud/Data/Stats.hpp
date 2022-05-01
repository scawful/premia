#ifndef STATS_HPP
#define STATS_HPP

#include "Premia.hpp"

namespace iex
{
    class Stats
    {
    private:
        std::string companyName; // Company name of the security
        double marketcap; // Market cap of the security calculated as shares outstanding * previous day close.
        double week52high; // Highest fully adjusted price observed during trading hours over the past 52 calendar weeks
        double week52low; // Lowest fully adjusted price observed during trading hours over the past 52 calendar weeks
        double week52highSplitAdjustOnly; // Highest split adjusted price observed during trading hours over the past 52 calendar weeks
        double week52lowSplitAdjustOnly; // Lowest split adjusted price observed during trading hours over the past 52 calendar weeks
        double week52change; // Based on 52 calendar weeks
        double sharesOutstanding; // Number of shares outstanding as the difference between issued shares and treasury shares. Investopedia
        double avg30Volume; // Average 30 day volume based on calendar days
        double avg10Volume; // Average 10 day volume based on calendar days
        int employees; 	
        double ttmEPS; // Trailing twelve month earnings per share. Investopedia
        double ttmDividendRate; // railing twelve month dividend rate per share
        double dividendYield; // The ratio of trailing twelve month dividend compared to the previous day close price. The dividend yield is represented as a percentage calculated as (ttmDividendRate) / (previous day close price) Investopedia
        std::string nextDividendDate; //	Announced ex date of the next dividend
        std::string exDividendDate; // Ex date of the last dividend
        std::string nextEarningsDate;; // 	string 	Announced next earnings report date
        double peRatio; // 	number 	Price to earnings ratio calculated as (previous day close price) / (ttmEPS).
        // Note: This endpoint is calculated on a trailing twelve month basis.
        double beta; //	number 	Beta is a measure used in fundamental analysis to determine the volatility of an asset or portfolio in relation to the overall market. Levered beta calculated with 1 year historical data and compared to SPY.
        double day200MovingAvg; // 	number 	Based on calendar days
        double day50MovingAvg; // 	number 	Based on calendar days
        double maxChangePercent; // 	number 	Based on calendar days
        double year5ChangePercent; // 	number 	Based on calendar days
        double year2ChangePercent; // 	number 	Based on calendar days
        double year1ChangePercent; // 	number 	Based on calendar days
        double ytdChangePercent; // 	number 	Based on calendar days
        double month6ChangePercent; // 	number 	Based on calendar days
        double month3ChangePercent; // 	number 	Based on calendar days
        double month1ChangePercent; //	number 	Based on calendar days
        double day30ChangePercent; // 	number 	Based on calendar days
        double day5ChangePercent;

    public:
        Stats()=default;

        double getMarketcap() const { return marketcap; }
        void setMarketcap(double marketcap_) { marketcap = marketcap_; }

        double getWeek52high() const { return week52high; }
        void setWeek52high(double week52high_) { week52high = week52high_; }

        double getWeek52low() const { return week52low; }
        void setWeek52low(double week52low_) { week52low = week52low_; }

        double getWeek52highSplitAdjustOnly() const { return week52highSplitAdjustOnly; }
        void setWeek52highSplitAdjustOnly(double week52highSplitAdjustOnly_) { week52highSplitAdjustOnly = week52highSplitAdjustOnly_; }

        double getWeek52lowSplitAdjustOnly() const { return week52lowSplitAdjustOnly; }
        void setWeek52lowSplitAdjustOnly(double week52lowSplitAdjustOnly_) { week52lowSplitAdjustOnly = week52lowSplitAdjustOnly_; }

        double getWeek52change() const { return week52change; }
        void setWeek52change(double week52change_) { week52change = week52change_; }

        double getSharesOutstanding() const { return sharesOutstanding; }
        void setSharesOutstanding(double sharesOutstanding_) { sharesOutstanding = sharesOutstanding_; }

        double getAvg30Volume() const { return avg30Volume; }
        void setAvg30Volume(double avg30Volume_) { avg30Volume = avg30Volume_; }

        double getAvg10Volume() const { return avg10Volume; }
        void setAvg10Volume(double avg10Volume_) { avg10Volume = avg10Volume_; }

        int getEmployees() const { return employees; }
        void setEmployees(int employees_) { employees = employees_; }

        double getTtmDividendRate() const { return ttmDividendRate; }
        void setTtmDividendRate(double ttmDividendRate_) { ttmDividendRate = ttmDividendRate_; }

        double getTtmEPS() const { return ttmEPS; }
        void setTtmEPS(double ttmEPS_) { ttmEPS = ttmEPS_; }

        double getDividendYield() const { return dividendYield; }
        void setDividendYield(double dividendYield_) { dividendYield = dividendYield_; }

        double getPeRatio() const { return peRatio; }
        void setPeRatio(double peRatio_) { peRatio = peRatio_; }

        double getBeta() const { return beta; }
        void setBeta(double beta_) { beta = beta_; }

        double getDay200MovingAvg() const { return day200MovingAvg; }
        void setDay200MovingAvg(double day200MovingAvg_) { day200MovingAvg = day200MovingAvg_; }

        double getDay50MovingAvg() const { return day50MovingAvg; }
        void setDay50MovingAvg(double day50MovingAvg_) { day50MovingAvg = day50MovingAvg_; }

        double getMaxChangePercent() const { return maxChangePercent; }
        void setMaxChangePercent(double maxChangePercent_) { maxChangePercent = maxChangePercent_; }

        double getYear5ChangePercent() const { return year5ChangePercent; }
        void setYear5ChangePercent(double year5ChangePercent_) { year5ChangePercent = year5ChangePercent_; }

        double getYear2ChangePercent() const { return year2ChangePercent; }
        void setYear2ChangePercent(double year2ChangePercent_) { year2ChangePercent = year2ChangePercent_; }

        double getYear1ChangePercent() const { return year1ChangePercent; }
        void setYear1ChangePercent(double year1ChangePercent_) { year1ChangePercent = year1ChangePercent_; }

        double getYtdChangePercent() const { return ytdChangePercent; }
        void setYtdChangePercent(double ytdChangePercent_) { ytdChangePercent = ytdChangePercent_; }

        double getMonth6ChangePercent() const { return month6ChangePercent; }
        void setMonth6ChangePercent(double month6ChangePercent_) { month6ChangePercent = month6ChangePercent_; }

        double getMonth3ChangePercent() const { return month3ChangePercent; }
        void setMonth3ChangePercent(double month3ChangePercent_) { month3ChangePercent = month3ChangePercent_; }

        double getMonth1ChangePercent() const { return month1ChangePercent; }
        void setMonth1ChangePercent(double month1ChangePercent_) { month1ChangePercent = month1ChangePercent_; }

        double getDay30ChangePercent() const { return day30ChangePercent; }
        void setDay30ChangePercent(double day30ChangePercent_) { day30ChangePercent = day30ChangePercent_; }

        double getDay5ChangePercent() const { return day5ChangePercent; }
        void setDay5ChangePercent(double day5ChangePercent_) { day5ChangePercent = day5ChangePercent_; }
    };
}

#endif // INSIDERSUMMARY_HPP
