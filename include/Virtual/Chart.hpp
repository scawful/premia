#ifndef Chart_hpp
#define Chart_hpp

#include "Library/ImGui.hpp"
#include "Library/ImPlot.hpp"
#include "Model/Chart/ChartModel.hpp"
#include "Premia.hpp"

namespace Premia {
class Chart {
 public:
  Chart() = default;
  virtual ~Chart() = default;

  virtual void importModel(std::shared_ptr<ChartModel>) = 0;
  virtual void fetchData(CRString ticker, tda::PeriodType ptype, int period_amt,
                         tda::FrequencyType ftype, int freq_amt, bool ext) = 0;
  virtual void update() = 0;
};
}  // namespace Premia

#endif