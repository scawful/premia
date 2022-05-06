#ifndef OptionChainView_hpp
#define OptionChainView_hpp

#include "Virtual/View.hpp"
#include "Model/Options/OptionsModel.hpp"

class OptionChainView: public View 
{
private:
    struct GEXEpochPair {
        double * epochArray;
        double * gammaArray;
        GEXEpochPair(double * epoch, double * gamma)
            : epochArray(epoch), gammaArray(gamma) { }
    };
    String symbol;
    EventMap events;
    ConsoleLogger logger;
    OptionsModel model;

    void drawSearch();
    void drawChain();
    void drawUnderlying();

public:
    String getName() override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;

private:
    ImPlotAxisFlags dateFlags = ImPlotAxisFlags_Time | ImPlotAxisFlags_NoLabel;
    ImPlotAxisFlags priceFlags = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
};

#endif