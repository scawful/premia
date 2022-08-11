#ifndef Chart_hpp
#define Chart_hpp

#include <implot/implot.h>
#include <implot/implot_internal.h>

#include <string>

#include "model/chart/chart_model.h"

namespace premia {
class Chart {
 public:
  Chart() = default;
  virtual ~Chart() = default;

  virtual void importModel(std::shared_ptr<ChartModel>) = 0;
  virtual void fetchData(const std::string& ticker, tda::PeriodType ptype,
                         int period_amt, tda::FrequencyType ftype, int freq_amt,
                         bool ext) = 0;
  virtual void Update() = 0;
};
}  // namespace premia

#endif