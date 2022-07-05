#ifndef LinePlotChart_hpp
#define LinePlotChart_hpp

#include <boost/lexical_cast.hpp>
#include <fstream>

#include "View/View.hpp"

namespace premia {
class LinePlotChart : public View {
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

  ArrayList<double> numDays;
  ArrayList<double> balances;
  ArrayList<String> dates;
  std::unordered_map<String, EventHandler> events;

  void initData();

 public:
  LinePlotChart();
  ~LinePlotChart() override;

  void update() override;
  String getName() override;
  void addLogger(const Logger &logger) override;
  void addEvent(CRString key, const EventHandler &event) override;
};
}  // namespace premia

#endif