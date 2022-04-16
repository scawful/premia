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

void RiskPremiaFrame::init_pie_chart()
{
    tda::Account account = premia->tda_interface.getDefaultAccount();
    if ( !symbols_array.empty() ) {
        symbols_array.clear();
    }
    for ( int i = 0; i < account.get_position_vector_size(); i++ ) {
        for ( const auto& [positionsKey, positionsValue] : account.get_position( i ) ) {
            if ( positionsKey == "symbol" ) {
                std::string str = positionsValue;
                symbols_array.push_back( str );
            }
        }
    }
    
    auto size = static_cast<int>(symbols_array.size());
    num_positions = size;
    positions_labels = new char*[size];
    positions_pie = new double[size];
    
    double sum = 0;
    for (int i = 0; i < size; i++) {
        std::string symbol = symbols_array[i];
        positions_pie[i] = boost::lexical_cast<double>(account.get_position_balances(symbol, "marketValue"));
        sum += positions_pie[i];
        positions_labels[i] = new char[symbol.size()];
        std::strncpy(positions_labels[i], symbol.c_str(), symbol.size());
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

    for (int i = 0; i < 400; i++) {
        oneYearDix[i] = dix[length - i - 1];
    }

    initialized = true;
}

void RiskPremiaFrame::draw_heatmap()
{
    static float values1[7][7] = { {0.8f, 2.4f, 2.5f, 3.9f, 0.0f, 4.0f, 0.0f},
                                        {2.4f, 0.0f, 4.0f, 1.0f, 2.7f, 0.0f, 0.0f},
                                        {1.1f, 2.4f, 0.8f, 4.3f, 1.9f, 4.4f, 0.0f},
                                        {0.6f, 0.0f, 0.3f, 0.0f, 3.1f, 0.0f, 0.0f},
                                        {0.7f, 1.7f, 0.6f, 2.6f, 2.2f, 6.2f, 0.0f},
                                        {1.3f, 1.2f, 0.0f, 0.0f, 0.0f, 3.2f, 5.1f},
                                        {0.1f, 2.0f, 0.0f, 1.4f, 0.0f, 1.9f, 6.3f} };
    static float scale_min = 0;
    static float scale_max = 6.3f;
    static const char* xlabels[] = { "C1","C2","C3","C4","C5","C6","C7" };
    static const char* ylabels[] = { "R1","R2","R3","R4","R5","R6","R7" };

    static ImPlotColormap map = ImPlotColormap_Viridis;
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map), ImVec2(225, 0), map)) {
        map = (map + 1) % ImPlot::GetColormapCount();
        // We bust the color cache of our plots so that item colors will
        // resample the new colormap in the event that they have already
        // been created. See documentation in implot.h.
        //BustColorCache("##Heatmap1");
        //BustColorCache("##Heatmap2");
    }

    ImGui::SameLine();
    ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");
    ImGui::SetNextItemWidth(225);
    ImGui::DragFloatRange2("Min / Max", &scale_min, &scale_max, 0.01f, -20, 20);
    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

    ImPlot::PushColormap(map);

    if (ImPlot::BeginPlot("##Heatmap1", ImVec2(225, 225), ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(NULL, NULL, axes_flags, axes_flags);
        ImPlot::SetupAxisTicks(ImAxis_X1, 0 + 1.0 / 14.0, 1 - 1.0 / 14.0, 7, xlabels);
        ImPlot::SetupAxisTicks(ImAxis_Y1, 1 - 1.0 / 14.0, 0 + 1.0 / 14.0, 7, ylabels);
        ImPlot::PlotHeatmap("heat", vix, 7, 7, scale_min, scale_max);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale", scale_min, scale_max, ImVec2(60, 225));
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

RiskPremiaFrame::~RiskPremiaFrame()
{
    delete[] positions_labels;
    delete positions_pie;
}

void RiskPremiaFrame::update()
{
    if (!initialized) {
        get_spx_gamma_exposure();
        if ((*tda_logged_in)) {
            init_pie_chart();
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Risk Premia", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Overview"))
        {
            ImGui::Spacing();
            ImGui::Indent();
            ImGui::Text("\n\t\t\t\t\tI can calculate the motions of the heavenly bodies, but not the madness of people.");
            ImGui::TextColored(ImVec4(224.f, 176.f, 255.f, 1.f), "\t\t\t\t\t\t\t\t\t\t\t\t\tSir Isaac Newton\n\n");
            ImGui::Unindent();
            ImGui::Spacing();
            ImGui::Separator();
            // Child 1: no border, enable horizontal scrollbar
            {
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.65f, 375), false, window_flags);
                ImGui::Text("General");
                if (ImGui::TreeNode("Getting Started")) 
                {
                    ImGui::Separator();
                    ImGui::Text("Welcome to Premia!");
                    ImGui::Separator();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Linking Accounts")) 
                {
                    ImGui::Separator(); ImGui::TextColored(ImVec4(42.f, 170.f, 138.f, 1.0f), "TDAmeritrade"); ImGui::Separator();
                    
                    ImGui::TextColored(ImVec4(0.f, 128.f, 128.f, 1.0f), "Coinbase"); ImGui::Separator();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Premia Pro Subscription")) 
                {
                    ImGui::TreePop();
                }


                ImGui::Text("Education");
                if (ImGui::TreeNode("Delta Hedging")) 
                {
                    ImGui::Separator();
                    ImGui::Text("Delta is an important hedge parameter for an options dealers portfolio.");
                    ImGui::TextWrapped("Options traders adjust delta frequently, making it close to zero, by trading the underlying asset.");
                    ImGui::TextWrapped("The Practioner Black-Scholes Delta is the partial derivative of the option price with respect to the underlying asset price with other variables, including the implied volatility, kept constant.");
                    ImGui::Separator();
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Gamma Exposure")) 
                {
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Standardizing Volatility")) 
                {
                    ImGui::Separator(); ImGui::Text("Average True Range"); ImGui::Separator();
                    ImGui::TextWrapped("One of the most common ways to measure volatility is in terms of the average true range of a trading session.");

                    ImGui::Separator(); ImGui::Text("Historical Volatility"); ImGui::Separator();
                    ImGui::TextWrapped("Statistical or realized volatility.");

                    ImGui::Separator(); ImGui::Text("Sigma Spike"); ImGui::Separator();
                    ImGui::TextWrapped("Standard deviation of the past 20-day return.");
                    ImGui::BulletText("Calculate returns for the price series");
                    ImGui::BulletText("Calculate the 20-day standard deviation of the returns.");
                    ImGui::BulletText("Base Variation = 20-day std dev * closing price");
                    ImGui::BulletText("Spike = (Close[i] - Close[i - 1]) * Previous Base Variation");
                    ImGui::Separator();
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Bonds and Interest Rates")) 
                {
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Futures and Futures Options")) 
                {
                    ImGui::TreePop();
                }
                ImGui::Separator();
                ImGui::Text("Development");
                if (ImGui::TreeNode("Changelog")) {
                    ImGui::Separator();
                    ImGui::BulletText("V0.03 \tExtensive upgrades to API interfaces and more GUI enhancements.");
                    ImGui::BulletText("V0.02 \tAdded support for CoinbasePro and created basic GUI layout.");
                    ImGui::BulletText("V0.01 \tStarted Premia using TDAmeritrade API and SDL with ImGUI.");
                    ImGui::Separator();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Credits")) {
                    ImGui::Separator();
                    ImGui::TextWrapped("I'd like to thank myself, I'm the only one who could have made this possible - scawful");
                    ImGui::Separator();
                    ImGui::TreePop();
                }
                
                ImGui::EndChild();
            }

            ImGui::SameLine();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
            ImGui::SameLine();

            // Child 2: rounded border
            {
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::BeginChild("ChildR", ImVec2(0, 375), true, window_flags);
                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("Menu"))
                    {
                        
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenuBar();
                }

                /**
                 * @brief Pie Chart 
                 * 
                 */
                if (ImPlot::BeginPlot("##Pie1", ImVec2(250, 250), ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) {
                    ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
                    ImPlot::SetupAxesLimits(0, 1, 0, 1);
                    ImPlot::PlotPieChart(positions_labels, positions_pie, num_positions, 0.5, 0.5, 0.4, false, "%.2f");
                    ImPlot::EndPlot();
                }

                /**
                 * @brief Button Table
                 * 
                 */
                if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
                {
                    for (int i = 0; i < 30; i++)
                    {
                        char buf[32];
                        sprintf(buf, "%03d", i);
                        ImGui::TableNextColumn();
                        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                    }
                    ImGui::EndTable();
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();
            }

            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

            // draw_heatmap();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Volatility Index"))
        {
            if (ImPlot::BeginPlot("SPX Volatility Index", ImVec2(-1, io.DisplaySize.x * 0.35f)))
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
            if (ImPlot::BeginPlot("SPX Dark Index", ImVec2(-1, io.DisplaySize.x * 0.35f)))
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
            if (ImPlot::BeginPlot("SPX Gamma Exposure Index", ImVec2(-1, io.DisplaySize.x * 0.35f)))
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
            if (ImPlot::BeginPlot("1-day Gamma Volatiltiy", ImVec2(-1, io.DisplaySize.x * 0.35f)))
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
