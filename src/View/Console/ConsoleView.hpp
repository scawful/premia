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
    int                   HistoryPos = -1;           // -1: new line, 0..History.Size-1 browsing history.
    char                  InputBuf[256];
    String                title;
    Logger                logger;
    EventMap              events;
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    ImGuiTextFilter       Filter;

    void clearLog();
    void executeCommand(const char* command_line);
    void drawScreen();

public:
    ConsoleView();
    ~ConsoleView();

    String getName() override;

    int TextEditCallback(ImGuiInputTextCallbackData* data);

    void addLog(const char* fmt, ...);
    void addLogStd(CRString data);
    void addLogger(const Logger& logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;
};

#endif