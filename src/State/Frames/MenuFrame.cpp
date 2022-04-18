#include "MenuFrame.hpp"
#include "../PrimaryState.hpp"
#include "../StreamState.hpp"
#include "../DemoState.hpp"

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void MenuFrame::init_hot_keys()
{
    std::ifstream fileInput("../assets/hotkeys.csv");

    if (!fileInput.good()) {
        SDL_Log("file not opened");
    }

    int i = 0;
    std::string temp, key, val, line;
    if(fileInput.good())
    {
        while(std::getline(fileInput, line))
        {
            // Create a stringstream from line
            std::stringstream ss(line);
            std::getline(ss, key, ',');
            
            std::getline(ss, val);
            hot_keys[key] = val;
        }
    }
}

void MenuFrame::draw_style_editor()
{
    ImGuiStyle& style = ImGui::GetStyle();
    static ImGuiStyle ref_saved_style;

    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == nullptr)
        ref_saved_style = style;
    init = false;
    if (ref == nullptr)
        ref = &ref_saved_style;

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

    if (ImGui::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    ImGui::ShowFontSelector("Fonts##Selector");

    // Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
    if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder",  &border)) { style.FrameBorderSize  = border ? 1.0f : 0.0f; } }
    ImGui::SameLine();
    { bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder",  &border)) { style.PopupBorderSize  = border ? 1.0f : 0.0f; } }

    // Save/Revert button
    if (ImGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    ImGui::SameLine();
    if (ImGui::Button("Revert Ref"))
        style = *ref;
    ImGui::SameLine();
    HelpMarker(
        "Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
        "Use \"Export\" below to save them somewhere.");

    ImGui::Separator();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Sizes"))
        {
            ImGui::Text("Main");
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            ImGui::Text("Borders");
            ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::Text("Rounding");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::Text("Alignment");
            ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = window_menu_button_position - 1;
            ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
            ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
            ImGui::Text("Safe Area Padding");
            ImGui::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (ImGui::Button("Export"))
            {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const ImVec4& col = style.Colors[i];
                    const char* name = ImGui::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
                            name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                ImGui::LogFinish();
            }
            ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = 0;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
            if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            HelpMarker(
                "In the color list:\n"
                "Left-click on color square to open color picker,\n"
                "Right-click to open edit options menu.");

            ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
            ImGui::PushItemWidth(-160);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                    // so instead of "Save"/"Revert" you'd use icons!
                    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                    ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
                }
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Fonts"))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
            ImGui::ShowFontAtlas(atlas);

            // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
            // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
            const float MIN_SCALE = 0.3f;
            const float MAX_SCALE = 2.0f;
            HelpMarker(
                "Those are old settings provided for convenience.\n"
                "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
                "rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
                "Using those settings here will give you poor quality results.");
            static float window_scale = 1.0f;
            ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
            if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp)) // Scale only this window
                ImGui::SetWindowFontScale(window_scale);
            ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rendering"))
        {
            ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            ImGui::SameLine();
            HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

            ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
            ImGui::SameLine();
            HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

            ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
            ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

            // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
            ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            if (ImGui::IsItemActive())
            {
                ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
                ImGui::BeginTooltip();
                ImGui::TextUnformatted("(R = radius, N = number of segments)");
                ImGui::Spacing();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                const float min_widget_width = ImGui::CalcTextSize("N: MMM\nR: MMM").x;
                for (int n = 0; n < 8; n++)
                {
                    const float RAD_MIN = 5.0f;
                    const float RAD_MAX = 70.0f;
                    const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

                    ImGui::BeginGroup();

                    ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

                    const float canvas_width = std::max(min_widget_width, rad * 2.0f);
                    const float offset_x     = floorf(canvas_width * 0.5f);
                    const float offset_y     = floorf(RAD_MAX);

                    const ImVec2 p1 = ImGui::GetCursorScreenPos();
                    draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
                    ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

                    ImGui::EndGroup();
                    ImGui::SameLine();
                }
                ImGui::EndTooltip();
            }
            ImGui::SameLine();
            HelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");

            ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
            ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); HelpMarker("Additional alpha multiplier for disabled items (multiply over current value of Alpha).");
            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopItemWidth();
}

MenuFrame::MenuFrame() : Frame()
{
    this->current_frame = SubFrame::PREMIA_HOME;
    this->free_mode = false;
    this->portfolio_view = true;
    this->watchlist_view = true;
    this->console_view = true;
    init_hot_keys();
}

bool MenuFrame::portfolioView() const 
{
    return portfolio_view;
}
bool MenuFrame::consoleView() const 
{
    return console_view;
}
bool MenuFrame::watchlistView() const 
{
    return watchlist_view;
}

bool MenuFrame::freeMode() const 
{
    return free_mode;
}

void MenuFrame::set_title(const std::string & title)
{
    this->title_string = title;
}

MenuFrame::SubFrame MenuFrame::get_current_frame() const
{
    return this->current_frame;
}

void MenuFrame::update()
{
    static bool about = false;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Premia Home")) {
                current_frame = MenuFrame::SubFrame::PREMIA_HOME;
            }
            ImGui::MenuItem("New Workspace", "CTRL + N");
            ImGui::Separator();
            ImGui::MenuItem("Open Workspace", "CTRL + O");
            if (ImGui::BeginMenu("Open Recent")) {
                ImGui::MenuItem("None");
                ImGui::EndMenu();
            }
            ImGui::Separator();
            ImGui::MenuItem("Save Workspace", "CTRL + S");
            ImGui::Separator();
            if (ImGui::BeginMenu("Preferences"))
            {
                ImGui::MenuItem("Private Balances", "", &getPremia()->halext_interface.privateBalance);
                ImGui::Separator();

                static int n = 0;
                ImGui::Combo("Default Account", &n, "Yes\0No\0Maybe\0\0");
                ImGui::Separator();

                ImGui::Text("Hot Keys");
                ImGui::BeginChild("Hot Keys", ImVec2(0, 100), true);
                for ( const auto & [key, value] : hot_keys ) {
                    ImGui::Text("%s  %s", key.c_str(), value.c_str());
                }
                ImGui::EndChild();

                ImGui::EndMenu();
            }

            if ( ImGui::MenuItem("Quit", "ESC") ) {
                getPremia()->quit();
            }

            ImGui::EndMenu();
        }

        /*
        *   Edit Sub Menu
        * 
        */
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo", "CTRL + Z");
            ImGui::MenuItem("Redo", "CTRL + Y");
            ImGui::Separator();
            ImGui::MenuItem("Cut", "CTRL + X");
            ImGui::MenuItem("Copy", "CTRL + C");
            ImGui::MenuItem("Paste", "CTRL + V");
            ImGui::MenuItem("Delete", "DEL");
            ImGui::Separator();
            if (ImGui::BeginMenu("Style")) {
                draw_style_editor(); 
                ImGui::EndMenu();       
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {         
            if (ImGui::MenuItem("Service Login")) {
                current_frame = SubFrame::LOGIN;
            }
            ImGui::Separator();
            ImGui::MenuItem("General Chat");
            ImGui::MenuItem("Script Editor");
            ImGui::Separator();
            ImGui::MenuItem("Console View", "", & console_view);
            ImGui::MenuItem("Portfolio View", "", &portfolio_view);
            ImGui::MenuItem("Watchlist View", "", &watchlist_view);
            ImGui::Separator();
            ImGui::MenuItem("Free Mode", "", &free_mode);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Trade"))
        {
            if ( ImGui::MenuItem("Place Order") ){
                current_frame = SubFrame::TRADING;
            }
            ImGui::MenuItem("Replace Order");
            ImGui::MenuItem("Cancel Order");
            ImGui::Separator();
            ImGui::MenuItem("Get Order");
            ImGui::Separator();
            if (ImGui::MenuItem("Option Chain")) {
                current_frame = SubFrame::OPTION_CHAIN;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Charts")) 
        {
            if (ImGui::MenuItem("Market Overview")) {
                current_frame = SubFrame::MARKET_OVERVIEW;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Candle Chart")) {
                current_frame = SubFrame::CANDLE_CHART;
            } 
            if (ImGui::MenuItem("Line Chart")) {
                current_frame = SubFrame::LINE_PLOT;
            }
            ImGui::MenuItem("Bar Chart");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Research"))
        {   
            ImGui::MenuItem("Daily Treasury Rates", "PRO");
            ImGui::MenuItem("Federal Funds Rate", "PRO");
            ImGui::MenuItem("Unemployment Rate", "PRO");
            ImGui::MenuItem("US Recession Probabilities", "PRO");
            ImGui::Separator();
            ImGui::MenuItem("Consumer Price Index", "PRO");
            ImGui::MenuItem("Industrial Production Index", "PRO");
            ImGui::Separator();
            if (ImGui::MenuItem("Risk Appetite Index")) {
                current_frame = SubFrame::RISK_APPETITE;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Analyze"))
        {
            ImGui::MenuItem("Fundamentals");
            if (ImGui::MenuItem("Market Movers")) {
                current_frame = SubFrame::MARKET_MOVERS;
            }
            ImGui::Separator();
            ImGui::MenuItem("Insider Roster", "PRO");
            ImGui::MenuItem("Insider Summary", "PRO");
            if (ImGui::MenuItem("Insider Transactions", "PRO")) {
                current_frame = SubFrame::INSIDER_TRANSACTIONS;
            }
            if (ImGui::MenuItem("Fund Ownership", "PRO")) {
                current_frame = SubFrame::FUND_OWNERSHIP;
            }
            ImGui::Separator();
            ImGui::MenuItem("Retail Money Funds", "PRO");
            ImGui::MenuItem("Institutional Money Funds", "PRO");
            if (ImGui::MenuItem("Institutional Ownership", "PRO")) {
                current_frame = SubFrame::INSTITUTIONAL_OWNERSHIP;
            }
            ImGui::Separator();
            ImGui::MenuItem("Largest Trades", "PRO");
            ImGui::MenuItem("Market Volume (U.S.)", "PRO");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Account")) 
        {
            ImGui::MenuItem("Sync Data");
            ImGui::Separator();
            ImGui::MenuItem("General Settings");
            ImGui::MenuItem("Graphical Settings");
            ImGui::Separator();
            ImGui::MenuItem("Activate Trial");
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::MenuItem("Service Login", "TDA")) {
                *getTDALoggedIn() = true;
                *getPublicMode() = false;
            }
            if (ImGui::MenuItem("Fetch Access Token", "TDA")) {
                getPremia()->tda_interface.fetchAccessToken();
            }
            if (ImGui::MenuItem("WebSocket StreamState")) {
                getPremia()->change(StreamState::instance());
            }
            if (ImGui::MenuItem("ImGui/ImPlot Demos")) {
                getPremia()->change(DemoState::instance());
            }      
            if (ImGui::MenuItem("Start WS Session")) {
                getPremia()->tda_interface.startStreamingSession();
            }
            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu("Help") )
        {
            ImGui::MenuItem("Get Started");
            ImGui::MenuItem("Tips and Tricks");
            if ( ImGui::MenuItem("About") )
                about = true;

            ImGui::EndMenu();
        }


        ImGui::EndMenuBar();
    }

    if ( about )
        ImGui::OpenPopup("About");

    if ( ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        ImGui::Text("Premia Version 0.3");
        ImGui::Text("Written by: Justin Scofield (scawful)");
        ImGui::Text("Dependencies: Boost, SDL2, ImGui, ImPlot, libCURL");
        ImGui::Text("Data provided by: CoinbasePro, IEXCloud, TDAmeritrade, SqueezeMetrics");

        if ( ImGui::Button("Close", ImVec2(120, 0)) ) { about = false; ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}