#include "Indicators.hpp"

using namespace Indicators;

void 
Volume::importData(ArrayList<double> data) {
    this->volume = data;
}

void 
Volume::update() {
    if (ImPlot::BeginPlot("Volume")) {
        ImPlot::PlotBars("Bars", volume.data(), 10, 0.7, 1);
        ImPlot::EndPlot();
    }
}

void 
BollingerBands::importData() {
    static float xs[1001], ys[1001], ys1[1001], ys2[1001], ys3[1001], ys4[1001];
    srand(0);
    for (int i = 0; i < 1001; ++i) {
        xs[i]  = i * 0.001f;
        ys[i]  = 0.25f + 0.25f * sinf(25 * xs[i]) * sinf(5 * xs[i]);
        ys1[i] = ys[i];
        ys2[i] = ys[i];
        ys3[i] = 0.75f + 0.2f * sinf(25 * xs[i]);
        ys4[i] = 0.75f + 0.1f * cosf(25 * xs[i]);
    }
    static float alpha = 0.25f;
    ImGui::DragFloat("Alpha",&alpha,0.01f,0,1);

    if (ImPlot::BeginPlot("Shaded Plots")) {
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, alpha);
        ImPlot::PlotShaded("Uncertain Data",xs,ys1,ys2,1001);
        ImPlot::PlotLine("Uncertain Data", xs, ys, 1001);
        ImPlot::PlotShaded("Overlapping",xs,ys3,ys4,1001);
        ImPlot::PlotLine("Overlapping",xs,ys3,1001);
        ImPlot::PlotLine("Overlapping",xs,ys4,1001);
        ImPlot::PopStyleVar();
        ImPlot::EndPlot();
    }
}

void 
BollingerBands::update()
{

}