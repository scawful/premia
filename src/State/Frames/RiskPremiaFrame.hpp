#ifndef RiskPremiaFrame_hpp
#define RiskPremiaFrame_hpp

#include "Frame.hpp"

class RiskPremiaFrame : public Frame
{
private:
    std::string title_string;
    int length;
    double gex[3500];
    double vix[3500];
    double dix[3500];
    double spx[3500];
    double gxv[3500];
    double date[3500];

    int num_positions;
    double* positions_pie;
    char** positions_labels;

    std::vector<std::string> symbols_array;

    double oneYearDix[400];

    double minVix, minGex, minDix, minSpx, minGxv;
    double maxVix, maxGex, maxDix, maxSpx, maxGxv;

    void init_pie_chart();
    void get_spx_gamma_exposure();

    void draw_heatmap();

public:
    RiskPremiaFrame();
    ~RiskPremiaFrame();
    
    void update();
};

#endif 