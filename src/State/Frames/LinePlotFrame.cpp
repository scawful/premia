#include "LinePlotFrame.hpp"

void LinePlotFrame::initData()
{
    std::ifstream fileInput("../assets/chart.csv");

    if (!fileInput.good()) {
        SDL_Log("file not opened");
    }

    int i = 0;
    std::string temp, word, line;
    if(fileInput.good())
    {
        while(std::getline(fileInput, line))
        {
            // Create a stringstream from line
            std::stringstream ss(line);
            std::getline(ss, word, ',');
            std::getline(ss, word);
            i++;
            this->x[i] = i;
            this->y[i] = boost::lexical_cast<double>(word); 
        }

    }
}

LinePlotFrame::LinePlotFrame()
{
    this->show_lines = true;
    this->show_fills = true;
    this->fill_ref = 0;
    this->shade_mode = 0;
    this->x = new double[365];
    this->y = new double[365];
    this->initData();
}

LinePlotFrame::~LinePlotFrame()
{
    delete [] x;
    delete [] y;
}

void LinePlotFrame::update() 
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

    ImGuiIO& io = ImGui::GetIO();
    if (ImPlot::BeginPlot("Equity Curve", ImVec2(io.DisplaySize.x * 0.73, io.DisplaySize.y * 0.59))) {
        ImPlot::SetupAxes("Date","Price");
        ImPlot::SetupAxesLimits(1,238,1200,6200);
        if (show_fills) {
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded("TDAmeritrade", x, y, 238, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref);
            ImPlot::PopStyleVar();
        }
        if (show_lines) {
            ImPlot::PlotLine("TDAmeritrade", x, y, 238);
        }
        ImPlot::EndPlot();
    }
}

void LinePlotFrame::render() 
{
    
}
