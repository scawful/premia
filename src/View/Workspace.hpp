#ifndef Workspace_hpp
#define Workspace_hpp

#include "Virtual/View.hpp"
#include "Core/PrimaryView.hpp"
#include "Login/LoginView.hpp"
#include "Menu/MenuView.hpp"
#include "Core/ConsoleView.hpp"
#include "Watchlist/WatchlistView.hpp"
#include "Chart/ChartView.hpp"
#include "Account/AccountView.hpp"

/**
 * @brief Customizable Workspaces for Charts, Watchlists, Portfolios, News, Console, and more 
 * 
 * @details Create a class that substitutes for ViewManager but allows for more customization
 *          Has no concrete main view, rather allows you to open an indeterminate amount of views
 *          There should still be some root view, perhaps a widget or toolbar that holds the main 
 *          buttons and functionality of inserting other views... 
 * 
 *          View classes are designed so that they do not initialize an ImGui window when they are created
 *          The view window is what we drag around and press the x button to close (but within the larger Premia SDL window)
 *          So the general flow of this is the following:
 * 
 *          ImGui::SetNextWindowSize(ImVec2(X,Y), ImGuiCond_Appearing);
 *          ImGui::Begin("<View-Name>", args...);
 *            view.update();
 *          ImGui::End();
 * 
 *          And this code can be called after pressing a button, inputting a console command, or a key combination... 
 * 
 */
class Workspace {
private:
    EventMap events;
    Logger logger;
    std::shared_ptr<View> rootView;

    // load workspace configurations from files ? 
    // map of views? sharing events? idk idk 

public:
    Workspace();
    
    void update();
};

#endif