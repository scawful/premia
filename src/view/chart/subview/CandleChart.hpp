#ifndef PREMIA_VIEW_CANDLECHART_H
#define PREMIA_VIEW_CANDLECHART_H

#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "model/chart/chart_model.h"
#include "view/chart/chart.h"

namespace premia {
class CandleChart : public Chart {
 private:
  std::string tickerSymbol;
  std::string quoteDetails;
  std::shared_ptr<ChartModel> model;

  int binary_search(const std::vector<double>& arr, int l, int r, double x);
  void drawCandles(float width_percent, int count, ImVec4 bullCol,
                   ImVec4 bearCol, bool tooltip);
  void drawCandleChart();

 public:
  void importModel(std::shared_ptr<ChartModel> newModel) override;
  void fetchData(const std::string &ticker, tda::PeriodType ptype, int period_amt,
                 tda::FrequencyType ftype, int freq_amt, bool ext) override;
  void update() override;
};
}  // namespace premia

#endif