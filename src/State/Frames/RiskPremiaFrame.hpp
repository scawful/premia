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

    double minVix, minGex, minDix, minSpx, minGxv;
    double maxVix, maxGex, maxDix, maxSpx, maxGxv;

    void get_spx_gamma_exposure();

public:
    RiskPremiaFrame();
    
    void update();
};

#endif 