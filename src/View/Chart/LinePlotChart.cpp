#include "LinePlotChart.hpp"

void LinePlotChart::initData()
{
    std::ifstream fileInput("assets/chart.csv");

    if (!fileInput.good()) {
        SDL_Log("file not opened");
    }

    int i = 0;
    String temp, word, line;
    if (fileInput.good()) {
        while (std::getline(fileInput, line)) {
            // Create a std::istringstream from line
            std::istringstream ss(line);
            std::getline(ss, word, ',');
            std::getline(ss, word);
            i++;
            this->x[i] = i;
            this->y[i] = boost::lexical_cast<double>(word); 
            if (this->y[i] > high) {
                high = this->y[i];
            }

            if (low == 0.0) {
                low = this->y[i];
            }

            if (this->y[i] < low) {
                low = this->y[i];
            }

            num++;
        }

    }
}

LinePlotChart::LinePlotChart()
{
    this->show_lines = true;
    this->show_fills = true;
    this->fill_ref = 0;
    this->shade_mode = 0;
    this->x = new double[365];
    this->y = new double[365];
    this->initData();
}

LinePlotChart::~LinePlotChart()
{
    delete [] x;
    delete [] y;
}

void LinePlotChart::addAuth(CRString key , CRString token)
{
    // model.addAuth(key, token);
}

void LinePlotChart::addLogger(const Premia::ConsoleLogger & newLogger)
{
    // this->logger = newLogger;
}

void LinePlotChart::addEvent(CRString key, const Premia::EventHandler & event)
{
    this->events[key] = event;
}

void LinePlotChart::update() 
{
    ImGui::Checkbox("Lines",&show_lines); ImGui::SameLine();
    ImGui::Checkbox("Fills",&show_fills);
    if (show_fills) {
        ImGui::SameLine();
        if (ImGui::RadioButton("To -INF",shade_mode == 0))
            shade_mode = 0;
        ImGui::SameLine();
        if (ImGui::RadioButton("To +INF",shade_mode == 1))
            shade_mode = 1;
        ImGui::SameLine();
        if (ImGui::RadioButton("To Ref",shade_mode == 2))
            shade_mode = 2;
        if (shade_mode == 2) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::DragFloat("##Ref",&fill_ref, 1, -100, 500);
        }
    }

    const ImGuiIO & io = ImGui::GetIO();
    if (ImPlot::BeginPlot("Equity Curve", ImGui::GetContentRegionAvail())) {
        ImPlot::SetupAxes("Date","Price");
        ImPlot::SetupAxesLimits(1, num + 1, low, high);
        if (show_fills) {
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded("TDAmeritrade", x, y, num + 1 , shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref);
            ImPlot::PopStyleVar();
        }
        if (show_lines) {
            ImPlot::PlotLine("TDAmeritrade", x, y, num + 1);
        }
        ImPlot::EndPlot();
    }
}
