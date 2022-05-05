#include "ChartView.hpp"

void 
ChartView::initChart()
{
    halext::ChartType chartType = halext::HLXT::getInstance().getSelectedChart();
    if (chartType == halext::ChartType::CANDLESTICK) {
        this->currentChart = "CANDLESTICK";
        this->charts["CANDLESTICK"] = std::make_shared<CandleChart>();
        this->charts["CANDLESTICK"]->importModel(model);
        this->indicators["VOLUME"] = std::make_shared<Volume>();
        this->indicators["VOLUME"]->importData(model->getVolumeVector());
    } else if (chartType == halext::ChartType::LINEPLOT) {
        this->currentChart = "LINEPLOT";
        //this->charts["currentChart"] = std::make_unique<LinePlotChart>();
    }
    isInit = true;
}

void
ChartView::drawChartSettings()
{
    if (ImGui::BeginTable("Chart Settings", 13, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableNextColumn(); ImGui::Text("Symbol");
        ImGui::TableNextColumn(); ImGui::SetNextItemWidth(50.f); ImGui::InputText("##chartEnterSymbol", &tickerSymbol, ImGuiInputTextFlags_CharsUppercase);
        ImGui::TableNextColumn(); ImGui::Text("Period");
        ImGui::TableNextColumn(); ImGui::SetNextItemWidth(55.f); ImGui::Combo("##Period", &period_type, "Day\0Month\0Year\0YTD\0");
        ImGui::TableNextColumn(); ImGui::Text("Type");
        ImGui::TableNextColumn(); ImGui::SetNextItemWidth(50.f); ImGui::Combo("##type", &period_amount, " 1\0 2\0 3\0 4\0 5\0 6\0 10\0 15\0 20\0");
        ImGui::TableNextColumn(); ImGui::Text("Frequency");
        ImGui::TableNextColumn(); ImGui::SetNextItemWidth(75.f); ImGui::Combo("##frequency", &frequency_type, "Minute\0Daily\0Weekly\0Monthly\0");
        ImGui::TableNextColumn(); ImGui::Text("Amount");
        ImGui::TableNextColumn(); ImGui::SetNextItemWidth(50.f); ImGui::Combo("##amount", &frequency_amount, " 1\0 5\0 10\0 15\0 30\0");
        ImGui::TableNextColumn();
        if (ImGui::Button("Search") && !tickerSymbol.empty()) {
            this->charts[currentChart]->fetchData(tickerSymbol, tda::PeriodType(period_type), period_amount, 
                                                                  tda::FrequencyType(frequency_type), frequency_amount, true);  
        }
        ImGui::EndTable();
    }
}

String 
ChartView::getName() {
    return "Chart";
}

void 
ChartView::addLogger(const ConsoleLogger & newLogger) {
    this->logger = newLogger;
}

void 
ChartView::addEvent(CRString key, const EventHandler & event) {
    this->events[key] = event;
}

void 
ChartView::update() {
    if (!isInit) {
        initChart();
    }
    drawChartSettings();
    charts.at(currentChart)->update();
    indicators["VOLUME"]->update();
}
