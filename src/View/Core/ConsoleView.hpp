#ifndef ConsoleView_hpp
#define ConsoleView_hpp

#include "../View.hpp"

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
    Premia::ConsoleLogger logger;
    std::unordered_map<String, Premia::EventHandler> events;

    void drawScreen();

public:
    ConsoleView();
    ~ConsoleView();

    int TextEditCallback(ImGuiInputTextCallbackData* data);

    void addLog(const char* fmt, ...);
    void addLogStd(CRString data);
    void addAuth(CRString key , CRString token) override;
    void addLogger(const Premia::ConsoleLogger & logger) override;
    void addEvent(CRString key, const Premia::EventHandler & event) override;
    void update() override;
};

#endif