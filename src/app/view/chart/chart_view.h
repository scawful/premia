#ifndef ChartView_hpp
#define ChartView_hpp

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "metatypes.h"
#include "premia/core/application/workflow_models.hpp"
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
  void SetActiveAccountId(const std::string& account_id);
  void SetActivePresetId(const std::string& preset_id);
  auto GetActivePresetId() const -> const std::string&;
  void Update() override;

 private:
  void initChart();
  void DrawChart();
  void DrawChartSettings();
  void DrawChartPresets();
  void DrawCoreContractPreview();
  void DrawOverlayControls();
  void DrawStatsStrip(const core::application::QuoteDetail& quote,
                      const core::application::ChartScreenData& chart);
  void FetchChartData();
  void ApplyPreset(const std::string& preset_id);
  void LoadAnnotationState();
  void PersistAnnotationState() const;
  void RefreshOverlayMarkers();
  auto AnnotationStorageKey() const -> std::string;
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
  std::string active_account_id_;
  std::string annotation_label_;
  std::string annotation_price_;
  std::string trade_entry_price_;
  std::string trade_stop_price_;
  std::string trade_target_price_;
  std::string editable_order_id_;
  std::string order_edit_message_;
  std::string currentChart;
  std::function<void(const std::string&)> symbol_change_handler_;
  std::unordered_map<std::string, std::vector<ChartOverlayMarker>> manual_annotations_;
  bool annotation_state_loaded_ = false;
  bool live_chart_replace_enabled_ = false;
  int selected_editable_order_index_ = 0;
  float editable_order_price_ = 0.0f;
  std::vector<core::application::OrderRecordData> editable_orders_;

  EventMap events;
  ChartMap charts;
  std::shared_ptr<ChartModel> model = std::make_shared<ChartModel>();

  Logger logger;
};
}  // namespace premia

#endif
