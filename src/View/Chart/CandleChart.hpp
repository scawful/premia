#ifndef CandleChart_hpp
#define CandleChart_hpp

#include "Chart.hpp"
#include "../../Model/Chart/ChartModel.hpp"

class CandleChart: public Chart
{
private:
    String tickerSymbol;
    ChartModel model;

    int binary_search(const ArrayList<double> arr, int l, int r, double x);
    void drawCandles(float width_percent, int count, ImVec4 bullCol, ImVec4 bearCol, bool tooltip);
    void drawCandleChart();

public:
    void update() override;

};

#endif