#ifndef ConsoleView_hpp
#define ConsoleView_hpp

#include "Virtual/View.hpp"
#include "Interface/TDA.hpp"

class ConsoleView : public View
{
private:
    bool initialized = false;
    bool guiDemo = false;
    bool plotDemo = false;
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;
    String           title;
    bool                  copy_to_clipboard = false;

    void clearLog();
    void executeCommand(const char* command_line);
    Logger logger;
    std::unordered_map<String, EventHandler> events;

    void drawScreen();

public:
    ConsoleView();
    ~ConsoleView();

    int TextEditCallback(ImGuiInputTextCallbackData* data);

    void addLog(const char* fmt, ...);
    void addLogStd(CRString data);
        String getName() override;
    void addLogger(const Logger& logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;
};

#endif