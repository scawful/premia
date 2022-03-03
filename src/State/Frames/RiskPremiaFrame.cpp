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
        std::cout << row << std::endl;
        std::istringstream ss(row);

        std::string imported_date;
        std::getline(ss, imported_date, ',');
        std::cout << imported_date << std::endl;

        // std::tm tmTime = {};
        // memset(&tmTime, 0, sizeof(tmTime));
        // strftime(imported_date.data(), imported_date.size(), "%m/%d/%Y", &tmTime);
        // date[i] = mktime(&tmTime);
        date[i] = i;

        std::string gexStr;
        std::getline(ss, gexStr, ',');
        std::cout << gexStr << std::endl;
        try {
            gex[i] = stod(gexStr);
        }
        catch (std::invalid_argument& e) {
            gex[i] = 0;
        }

        std::string gxvStr;
        std::getline(ss, gxvStr, ',');
        std::cout << gxvStr << std::endl;
        try {
            gxv[i] = stod(gxvStr);
        }
        catch (std::invalid_argument& e) {
            gxv[i] = 0;
        }

        std::string vixStr;
        std::getline(ss, vixStr, ',');
        std::cout << vixStr << std::endl;
        try {
            vix[i] = stod(vixStr);
        }
        catch (std::invalid_argument& e) {
            vix[i] = 0;
        }

        std::string dixStr;
        std::getline(ss, dixStr, ',');
        std::cout << dixStr << std::endl;
        try {
            dix[i] = stod(dixStr);
        }
        catch (std::invalid_argument& e) {
            dix[i] = dix[i - 1];
        }

        std::string spxStr;
        std::getline(ss, spxStr, ',');
        std::cout << spxStr << std::endl;
        try {
            spx[i] = stod(spxStr);
        }
        catch (std::invalid_argument& e) {
            spx[i] = 0;
        }

        i++;
    }
    length = i;

    reverse_array(gex, 0, length);
    reverse_array(gxv, 0, length);
    reverse_array(vix, 0, length);
    reverse_array(dix, 0, length);
    reverse_array(spx, 0, length);
}

RiskPremiaFrame::RiskPremiaFrame() : Frame()
{
    get_spx_gamma_exposure();
    //   std::thread t(&RiskPremiaFrame::get_spx_gamma_exposure, RiskPremiaFrame());
    //   t.join();
}

void RiskPremiaFrame::update()
{

    ImGui::Text("Risk Premia Hedging Flows Overview");
    ImGui::Text("Managing equity through convex and reflexive analysis of Gamma exposure on options dealer positoning.");
    ImGuiIO& io = ImGui::GetIO();
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Risk Premia", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Overview"))
        {
            if (ImPlot::BeginPlot("SPX Dark Index", ImVec2(-1, io.DisplaySize.x * 0.35)))
            {
                ImPlot::SetupAxes("Date", "SPX");
                ImPlot::SetupAxesLimits(0, 3500, 500, 5000);
                ImPlot::SetupAxis(ImAxis_Y2, "DIX", ImPlotAxisFlags_AuxDefault);
                ImPlot::SetupAxisLimits(ImAxis_Y2, 30, 70);

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
                ImPlot::PlotLine("SPX", date, spx, length);
                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
                ImPlot::PlotLine("DIX", date, dix, length);
                ImPlot::EndPlot();
            }


            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Gamma Exposure"))
        {
            if (ImPlot::BeginPlot("SPX Gamma Exposure Index", ImVec2(-1, io.DisplaySize.x * 0.35)))
            {
                ImPlot::SetupAxes("Date", "SPX");
                ImPlot::SetupAxesLimits(1, 3500, 500, 5000);
                ImPlot::SetupAxis(ImAxis_Y2, "GEX", ImPlotAxisFlags_AuxDefault);
                ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 100);

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
                ImPlot::PlotLine("SPX", date, spx, length);
                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
                ImPlot::PlotLine("GEX", date, gex, length);
                ImPlot::EndPlot();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Gamma Volatility"))
        {
            if (ImPlot::BeginPlot("1-day Gamma Volatiltiy", ImVec2(-1, io.DisplaySize.x * 0.35)))
            {
                ImPlot::SetupAxes("Date", "SPX");
                ImPlot::SetupAxesLimits(1, 3500, 500, 5000);
                ImPlot::SetupAxis(ImAxis_Y2, "GXV", ImPlotAxisFlags_AuxDefault);
                ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 100);

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
                ImPlot::PlotLine("SPX", date, spx, length);                
                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
                ImPlot::PlotLine("GXV", date, gxv, length);
                ImPlot::EndPlot();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("SPX Volatility Landscape"))
        {
            if (ImPlot::BeginPlot("SPX Volatility Index", ImVec2(-1, io.DisplaySize.x * 0.35)))
            {
                ImPlot::SetupAxes("Date", "SPX");
                ImPlot::SetupAxesLimits(1, 3500, 500, 5000);
                ImPlot::SetupAxis(ImAxis_Y2, "VIX", ImPlotAxisFlags_AuxDefault);
                ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 100);

                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
                ImPlot::PlotLine("SPX", date, spx, length);
                ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
                ImPlot::PlotLine("VIX", date, vix, length);
                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
