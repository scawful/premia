#ifndef Chart_hpp
#define Chart_hpp

// #include "Library/ImGui.hpp"
#include "../libs/implot/implot.h"
#include "../libs/implot/implot_internal.h"
#include "Model/Chart/ChartModel.hpp"
#include "Premia.hpp"

namespace premia {
class Chart {
 public:
  Chart() = default;
  virtual ~Chart() = default;

  virtual void importModel(std::shared_ptr<ChartModel>) = 0;
  virtual void fetchData(CRString ticker, tda::PeriodType ptype, int period_amt,
                         tda::FrequencyType ftype, int freq_amt, bool ext) = 0;
  virtual void update() = 0;
};
}  // namespace premia

#endif