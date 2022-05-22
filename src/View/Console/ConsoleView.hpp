#ifndef ConsoleView_hpp
#define ConsoleView_hpp

#include "Virtual/View.hpp"
#include "Interface/TDA.hpp"

class ConsoleView : public View
{
private:
    bool                  initialized = false;
    bool                  guiDemo = false;
    bool                  plotDemo = false;
    bool                  AutoScroll = true;
    bool                  ScrollToBottom = false;
    bool                  copy_to_clipboard = false;
    bool                  reclaim_focus = false;
    bool                  DebugCallback = false;
    int                   HistoryPos = -1;           // -1: new line, 0..History.Size-1 browsing history.
    char                  InputBuf[256];
    String                title;
    Logger                logger;
    EventMap              events;
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    ImGuiTextFilter       Filter;
    ImGuiInputTextFlags   input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | 
                                             ImGuiInputTextFlags_CallbackCompletion | 
                                             ImGuiInputTextFlags_CallbackHistory;

    int TextEditCallback(ImGuiInputTextCallbackData* data);

    void clearLog();
    void addToHistory(const char* line);
    void executeCommand(const char* line);
    void displayCommands();
    void drawContextMenu();
    void drawConsole();

public:
    ConsoleView();
    ~ConsoleView();

    String getName() override;
    void addLog(const char* fmt, ...);
    void addLogStd(CRString data);
    void addLogger(const Logger& logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;
};

#endif