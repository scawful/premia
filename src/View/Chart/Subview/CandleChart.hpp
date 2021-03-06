#ifndef CandleChart_hpp
#define CandleChart_hpp

#include "View/Chart/Chart.hpp"
#include "Model/Chart/ChartModel.hpp"

namespace premia {
class CandleChart : public Chart {
 private:
  String tickerSymbol;
  String quoteDetails;
  std::shared_ptr<ChartModel> model;

  int binary_search(const ArrayList<double>& arr, int l, int r, double x);
  void drawCandles(float width_percent, int count, ImVec4 bullCol,
                   ImVec4 bearCol, bool tooltip);
  void drawCandleChart();

 public:
  void importModel(std::shared_ptr<ChartModel> newModel) override;
  void fetchData(CRString ticker, tda::PeriodType ptype, int period_amt,
                 tda::FrequencyType ftype, int freq_amt, bool ext) override;
  void update() override;
};
}  // namespace premia

#endif