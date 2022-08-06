#ifndef LinePlotChart_hpp
#define LinePlotChart_hpp

#include <boost/lexical_cast.hpp>
#include <fstream>

#include "view/view.h"

namespace premia {
class LinePlotChart : public View {
 public:
  LinePlotChart();
  ~LinePlotChart() override;

  void Update() override;

  std::string getName() override;
  void addLogger(const Logger &logger) override;
  void addEvent(const std::string &key, const EventHandler &event) override;

 private:
  bool show_lines;
  bool show_fills;
  float fill_ref;
  int shade_mode;
  int num = 0;
  double high = 0;
  double low = 0;
  double *x;
  double *y;

  std::vector<double> numDays;
  std::vector<double> balances;
  std::vector<std::string> dates;
  std::unordered_map<std::string, EventHandler> events;

  void initData();
};
}  // namespace premia

#endif