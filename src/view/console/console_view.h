#ifndef ConsoleView_hpp
#define ConsoleView_hpp

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <string>

#include "metatypes.h"
#include "gfx/IconsMaterialDesign.h"
#include "singletons/TDA.hpp"
#include "singletons/TWS.hpp"
#include "view/view.h"

namespace premia {
class ConsoleView : public View {
 private:
  bool initialized = false;
  bool guiDemo = false;
  bool plotDemo = false;
  bool AutoScroll = true;
  bool ScrollToBottom = false;
  bool copy_to_clipboard = false;
  bool reclaim_focus = false;
  bool DebugCallback = false;
  int HistoryPos = -1;  // -1: new line, 0..History.Size-1 browsing history.
  char InputBuf[256];
  std::string title;
  Logger logger;
  EventMap events;
  ImVector<char*> Items;
  ImVector<const char*> Commands;
  ImVector<char*> History;
  ImGuiTextFilter Filter;
  ImGuiInputTextFlags input_text_flags =
      ImGuiInputTextFlags_EnterReturnsTrue |
      ImGuiInputTextFlags_CallbackCompletion |
      ImGuiInputTextFlags_CallbackHistory;

  int TextEditCallback(ImGuiInputTextCallbackData* data);

  void clearLog();
  void addToHistory(const char* line);
  void executeCommand(const char* line);
  void displayCommands();
  void DrawContextMenu();
  void DrawConsole();

 public:
  ConsoleView();
  ~ConsoleView();

  std::string getName() override;
  void addLog(const char* fmt, ...);
  void addLogStd(const std::string &data);
  void addLogger(const Logger& logger) override;
  void addEvent(const std::string &key, const EventHandler& event) override;
  void Update() override;
};
}  // namespace premia

#endif