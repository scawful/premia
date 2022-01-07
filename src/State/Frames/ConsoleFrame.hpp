#ifndef ConsoleFrame_hpp
#define ConsoleFrame_hpp

#include "Frame.hpp"

class ConsoleFrame : public Frame 
{
private:
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;
    std::string           title;
    bool                  p_open = true;

    void clearLog();
    void addLog(const char* fmt, ...);
    void executeCommand(const char* command_line);


public:
    int TextEditCallback(ImGuiInputTextCallbackData* data);

    ConsoleFrame();
    ~ConsoleFrame();

    void update();
};

#endif