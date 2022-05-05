#ifndef Chart_hpp
#define Chart_hpp

#include "Premia.hpp"
#include "Library/ImGui.hpp"
#include "Library/ImPlot.hpp"
#include "Model/Chart/ChartModel.hpp"

class Chart 
{
public:
    Chart()=default;
    virtual ~Chart()=default;

    virtual void importModel(std::shared_ptr<ChartModel>);
    virtual void fetchData(CRString ticker, tda::PeriodType ptype, int period_amt, 
                                          tda::FrequencyType ftype, int freq_amt, bool ext) = 0;
    virtual void update() = 0;
};

#endif