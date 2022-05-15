#ifndef ChartView_hpp
#define ChartView_hpp

#include "Virtual/View.hpp"
#include "Subview/CandleChart.hpp"

class ChartView 
    : public View {
    using ChartMap  = std::unordered_map<String, std::shared_ptr<Chart>>;
public:
    String getName() override;
    void addLogger(const Logger& logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;

private:
    int period_type = 2; 
    int period_amount = 0;
    int frequency_type = 1;
    int frequency_amount = 0;
    bool isInit = false;
    String tickerSymbol;
    String currentChart;

    EventMap events;
    ChartMap charts;
    std::shared_ptr<ChartModel> model = std::make_shared<ChartModel>();

    Logger logger;

    void initChart();
    void drawChart();
    void drawChartSettings();
};

#endif