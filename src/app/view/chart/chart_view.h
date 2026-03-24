#ifndef ChartView_hpp
#define ChartView_hpp

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "metatypes.h"
#include "view/chart/subview/candle_chart.h"
#include "view/view.h"

namespace premia {
class ChartView : public View {
  using ChartMap = std::unordered_map<std::string, std::shared_ptr<Chart>>;

 public:
  std::string getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(const std::string& key, const EventHandler& event) override;
   void SetTickerSymbol(const std::string& symbol);
   void SetSymbolChangeHandler(
       const std::function<void(const std::string&)>& handler);
  void Update() override;

 private:
  void initChart();
  void DrawChart();
  void DrawChartSettings();
  void DrawChartPresets();
  void DrawCoreContractPreview();
  void DrawStatsStrip(const core::application::QuoteDetail& quote,
                      const core::application::ChartScreenData& chart);
  void FetchChartData();
  void ApplyPreset(const std::string& preset_id);
  auto GetSelectedRangeLabel() const -> std::string;
  auto GetSelectedIntervalLabel() const -> std::string;

  
  int period_type = 2;
  int period_amount = 0;
  int frequency_type = 1;
  int frequency_amount = 0;
  bool isInit = false;
  bool pending_refresh_ = true;
  std::string active_preset_ = "1Y";
  std::string tickerSymbol = "AAPL";
  std::string currentChart;
  std::function<void(const std::string&)> symbol_change_handler_;

  EventMap events;
  ChartMap charts;
  std::shared_ptr<ChartModel> model = std::make_shared<ChartModel>();

  Logger logger;
};
}  // namespace premia

#endif
