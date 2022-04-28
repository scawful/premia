#ifndef ConsoleView_hpp
#define ConsoleView_hpp

#include "../View.hpp"

class ConsoleView : public View
{
private:
    bool initialized = false;
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;
    std::string           title;
    bool                  copy_to_clipboard = false;

    void clearLog();
    void addLog(const char* fmt, ...);
    void executeCommand(const char* command_line);

    std::unordered_map<std::string, VoidEventHandler> events;

    void drawScreen();

public:
    ConsoleView();
    ~ConsoleView();

    int TextEditCallback(ImGuiInputTextCallbackData* data);

    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void update() override;
};

#endif