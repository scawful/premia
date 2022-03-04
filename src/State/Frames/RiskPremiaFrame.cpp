#include "RiskPremiaFrame.hpp"

/* Function to reverse arr[] from start to end*/
void reverse_array(double arr[], int start, int end)
{
    while (start < end)
    {
        double temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

void RiskPremiaFrame::get_spx_gamma_exposure()
{
    std::string data = premia->client.get_spx_gex();
    std::istringstream ss(data);

    int i = 0;
    char c1, c2, c3, c4, c5;
    std::string header;
    ss >> header;

    while (!ss.eof())
    {
        std::string row;
        ss >> row;

        std::istringstream ss(row);
        std::string imported_date;
        std::getline(ss, imported_date, ',');


        date[i] = i;

        std::string gexStr;
        std::getline(ss, gexStr, ',');
        try {
            gex[i] = stod(gexStr);
        }
        catch (std::invalid_argument& e) {
            gex[i] = gex[i - 1];
        }

        if (gex[i] > maxGex)
            maxGex = gex[i];
        if (gex[i] < minGex)
            minGex = gex[i];

        std::string gxvStr;
        std::getline(ss, gxvStr, ',');
        try {
            gxv[i] = stod(gxvStr);
        }
        catch (std::invalid_argument& e) {
            gxv[i] = gxv[i - 1];
        }

        if (gxv[i] > maxGxv)
            maxGxv = gxv[i];
        if (gxv[i] < minGxv)
            minGxv = gxv[i];

        std::string vixStr;
        std::getline(ss, vixStr, ',');
        try {
            vix[i] = stod(vixStr);
        }
        catch (std::invalid_argument& e) {
            vix[i] = vix[i - 1];
        }

        if (vix[i] > maxVix)
            maxVix = vix[i];
        if (vix[i] < minVix)
            minVix = vix[i];

        std::string dixStr;
        std::getline(ss, dixStr, ',');
        try {
            dix[i] = stod(dixStr);
        }
        catch (std::invalid_argument& e) {
            dix[i] = dix[i - 1];
        }

        if (dix[i] > maxDix)
            maxDix = dix[i];
        if (dix[i] < minDix)
            minDix = dix[i];

        std::string spxStr;
        std::getline(ss, spxStr, ',');
        try {
            spx[i] = stod(spxStr);
        }
        catch (std::invalid_argument& e) {
            spx[i] = spx[i - 1];
        }

        if (spx[i] > maxSpx)
            maxSpx = spx[i];
        if (spx[i] < minSpx)
            minSpx = spx[i];

        i++;

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        progress += progress_dir * i * ImGui::GetIO().DeltaTime;

        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Loading Data");
    }
    length = i;

    reverse_array(gex, 0, length);
    reverse_array(gxv, 0, length);
    reverse_array(vix, 0, length);
    reverse_array(dix, 0, length);
    reverse_array(spx, 0, length);
    initialized = true;
}

RiskPremiaFrame::RiskPremiaFrame() : Frame()
{
    initialized = false;
    minVix = std::numeric_limits<double>::max();
    minGex = std::numeric_limits<double>::max();
    minSpx = std::numeric_limits<double>::max();
    minGxv = std::numeric_limits<double>::max();
    minDix = std::numeric_limits<double>::max();
    maxVix = 0;
    maxGex = 0;
    maxSpx = 0;
    maxGxv = 0;
    maxDix = 0;
}

void RiskPremiaFrame::update()
{
    if (!initialized) {
        get_spx_gamma_exposure();
    }


    ImGui::Text("Risk Premia Hedging Flows");
    ImGui::Text("Managing equity through convex and reflexive analysis of Gamma exposure on options dealer positoning.");
    ImGuiIO& io = ImGui::GetIO();
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Risk Premia", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Overview"))
        {

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Volatility Index"))
        {
            if (ImPlot::BeginPlot("SPX Volatility Index", ImVec2(-1, io.DisplaySize.x * 0.35)))
            {
                ImPlot::SetupAxes("Date", "SPX");
                ImPlot::SetupAxesLimits(0, length, minSpx, maxSpx);
                ImPlot::SetupAxis(ImAxis_Y2, "VIX", ImPlotAxisFlags_AuxDefault);
                ImPlot::SetupAxisLimits(ImAxis_Y2, minVix, maxVix);

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
                ImPlot::PlotLine("SPX", date, spx, length);
                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
                ImPlot::PlotLine("VIX", date, vix, length);
                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Dark Index")) {
            if (ImPlot::BeginPlot("SPX Dark Index", ImVec2(-1, io.DisplaySize.x * 0.35)))
            {
                ImPlot::SetupAxes("Date", "SPX");
                ImPlot::SetupAxesLimits(0, length, minSpx, maxSpx);
                ImPlot::SetupAxis(ImAxis_Y2, "DIX", ImPlotAxisFlags_AuxDefault);
                ImPlot::SetupAxisLimits(ImAxis_Y2, minDix, maxDix);

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
                ImPlot::PlotLine("SPX", date, spx, length);
                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
                ImPlot::PlotLine("DIX", date, dix, length);
                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Gamma Exposure Index"))
        {
            if (ImPlot::BeginPlot("SPX Gamma Exposure Index", ImVec2(-1, io.DisplaySize.x * 0.35)))
            {
                ImPlot::SetupAxes("Date", "SPX");
                ImPlot::SetupAxesLimits(0, length, minSpx, maxSpx);
                ImPlot::SetupAxis(ImAxis_Y2, "GEX", ImPlotAxisFlags_AuxDefault);
                ImPlot::SetupAxisLimits(ImAxis_Y2, minGex, maxGex);

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
                ImPlot::PlotLine("SPX", date, spx, length);
                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
                ImPlot::PlotLine("GEX", date, gex, length);
                ImPlot::EndPlot();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Gamma Volatility Index"))
        {
            if (ImPlot::BeginPlot("1-day Gamma Volatiltiy", ImVec2(-1, io.DisplaySize.x * 0.35)))
            {
                ImPlot::SetupAxes("Date", "SPX");
                ImPlot::SetupAxesLimits(0, length, minSpx, maxSpx);
                ImPlot::SetupAxis(ImAxis_Y2, "GXV", ImPlotAxisFlags_AuxDefault);
                ImPlot::SetupAxisLimits(ImAxis_Y2, minGxv, maxGxv);

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
                ImPlot::PlotLine("SPX", date, spx, length);                
                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
                ImPlot::PlotLine("GXV", date, gxv, length);
                ImPlot::EndPlot();
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}
