#ifndef PREMIA_VIEW_CANDLECHART_H
#define PREMIA_VIEW_CANDLECHART_H

#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "model/chart/chart_model.h"
#include "view/chart/chart.h"

namespace premia {
class CandleChart : public Chart {
 public:
  void Update() override;

  void importModel(std::shared_ptr<ChartModel> newModel) override;
  void fetchData(const std::string& ticker, tda::PeriodType ptype,
                 int period_amt, tda::FrequencyType ftype, int freq_amt,
                 bool ext) override;

 private:
  std::string tickerSymbol;
  std::string quoteDetails;
  std::shared_ptr<ChartModel> model;
  int hovered_index_ = -1;

  int binary_search(const std::vector<double>& arr, int l, int r, double x);
  int nearest_index(const std::vector<double>& arr, double x);
  void DrawReadoutCard();
  void DrawOverlayMarkers();
  void DrawCandles(float width_percent, int count, ImVec4 bullCol,
                   ImVec4 bearCol, bool tooltip);
  void DrawCandleChart();
};
}  // namespace premia

#endif
