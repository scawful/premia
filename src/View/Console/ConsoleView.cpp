#include "ConsoleView.hpp"

namespace premia {

// Portable helpers
static int Stricmp(const char* s1, const char* s2) {
  int d;
  while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
    s1++;
    s2++;
  }
  return d;
}

static int Strnicmp(const char* s1, const char* s2, int n) {
  int d = 0;
  while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
    s1++;
    s2++;
    n--;
  }
  return d;
}

static char* Strdup(const char* s) {
  IM_ASSERT(s);
  size_t len = strlen(s) + 1;
  void* buf = malloc(len);
  IM_ASSERT(buf);
  return (char*)memcpy(buf, (const void*)s, len);
}

static void Strtrim(char* s) {
  char* str_end = s + strlen(s);
  while (str_end > s && str_end[-1] == ' ') str_end--;
  *str_end = 0;
}

int ConsoleView::TextEditCallback(ImGuiInputTextCallbackData* data) {
  if (DebugCallback)
    addLog("cursor: %d, selection: %d-%d", data->CursorPos,
           data->SelectionStart, data->SelectionEnd);

  switch (data->EventFlag) {
    case ImGuiInputTextFlags_CallbackCompletion: {
      // TEXT COMPLETION
      // Locate beginning of current word
      const char* word_end = data->Buf + data->CursorPos;
      const char* word_start = word_end;

      while (word_start > data->Buf) {
        const char c = word_start[-1];
        if (c == ' ' || c == '\t' || c == ',' || c == ';') break;
        word_start--;
      }

      // Build a list of candidates
      ImVector<const char*> candidates;
      for (int i = 0; i < Commands.Size; i++)
        if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) ==
            0)
          candidates.push_back(Commands[i]);

      if (candidates.Size == 0) {
        // No match
        addLog("No match for \"%.*s\"!\n", (int)(word_end - word_start),
               word_start);
      } else if (candidates.Size == 1) {
        // Single match. Delete the beginning of the word and replace it
        // entirely so we've got nice casing.
        data->DeleteChars((int)(word_start - data->Buf),
                          (int)(word_end - word_start));
        data->InsertChars(data->CursorPos, candidates[0]);
        data->InsertChars(data->CursorPos, " ");
      } else {
        // Multiple matches. Complete as much as we can..
        // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and
        // "CLASSIFY" as matches.
        int match_len = (int)(word_end - word_start);
        for (;;) {
          int c = 0;
          bool all_candidates_matches = true;
          for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
            if (i == 0)
              c = toupper(candidates[i][match_len]);
            else if (c == 0 || c != toupper(candidates[i][match_len]))
              all_candidates_matches = false;
          if (!all_candidates_matches) break;
          match_len++;
        }

        if (match_len > 0) {
          data->DeleteChars((int)(word_start - data->Buf),
                            (int)(word_end - word_start));
          data->InsertChars(data->CursorPos, candidates[0],
                            candidates[0] + match_len);
        }

        // List matches
        addLog("Possible matches:\n");
        for (int i = 0; i < candidates.Size; i++)
          addLog("- %s\n", candidates[i]);
      }

      break;
    }
    case ImGuiInputTextFlags_CallbackHistory: {
      const int prev_history_pos = HistoryPos;
      if (data->EventKey == ImGuiKey_UpArrow) {
        if (HistoryPos == -1)
          HistoryPos = History.Size - 1;
        else if (HistoryPos > 0)
          HistoryPos--;
      } else if (data->EventKey == ImGuiKey_DownArrow) {
        if (HistoryPos != -1)
          if (++HistoryPos >= History.Size) HistoryPos = -1;
      }

      // A better implementation would preserve the data on the current input
      // line along with cursor position.
      if (prev_history_pos != HistoryPos) {
        const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, history_str);
      }
    }
  }
  return 0;
}

// Removes all contents from scrolling region
void ConsoleView::clearLog() {
  for (int i = 0; i < Items.Size; i++) free(Items[i]);
  Items.clear();
}

// Insert into history. First find match and delete it so it can be pushed to
// the back.
void ConsoleView::addToHistory(const char* line) {
  HistoryPos = -1;
  for (int i = History.Size - 1; i >= 0; i--)
    if (Stricmp(History[i], line) == 0) {
      free(History[i]);
      History.erase(History.begin() + i);
      break;
    }
  History.push_back(Strdup(line));
}

void ConsoleView::executeCommand(const char* command_line) {
  addLog("# %s\n", command_line);
  addToHistory(command_line);
  String commandString = command_line;

  // Process command
  if (Stricmp(command_line, "CLEAR") == 0) {
    clearLog();
  } else if (Stricmp(command_line, "HELP") == 0) {
    addLog("Commands:");
    for (int i = 0; i < Commands.Size; i++) addLog("- %s", Commands[i]);
  } else if (Stricmp(command_line, "HISTORY") == 0) {
    int first = History.Size - 10;
    for (int i = first > 0 ? first : 0; i < History.Size; i++)
      addLog("%3d: %s\n", i, History[i]);
  } else if (Stricmp(command_line, "GUIDEMO") == 0) {
    guiDemo = true;
  } else if (Stricmp(command_line, "PLOTDEMO") == 0) {
    plotDemo = true;
  } else if (Stricmp(command_line, "RUN_TWS") == 0) {
    tws::TWS::getInstance().runClient("127.0.0.1", 7496, 0);
  } else if (Stricmp(command_line, "CLOSE_SOCKET") == 0) {
    addLogStd("Ending WebSocket session...");
    tda::TDA::getInstance().sendSocketLogout();
  } else if (commandString.substr(0, 10) == "LOAD_QUOTE") {
    String ticker = commandString.substr(11, commandString.size());
    addLogStd("Opening WebSocket session and requesting QUOTE for " + ticker);
    tda::TDA::getInstance().sendChartRequestToSocket(logger, ticker);
  } else {
    addLog("Unknown command: '%s'\n", command_line);
  }

  ScrollToBottom =
      true;  // On command input, we scroll to bottom even if AutoScroll==false
}

void ConsoleView::displayCommands() {
  // Display every line as a separate entry so we can change their color or add
  // custom widgets.
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                      ImVec2(4, 1));  // Tighten spacing
  if (copy_to_clipboard) ImGui::LogToClipboard();

  for (int i = 0; i < Items.Size; i++) {
    const char* item = Items[i];
    if (!Filter.PassFilter(item)) continue;

    // Normally you would store more information in your item than just a
    // string. (e.g. make Items[] an array of structure, store color/type etc.)
    ImVec4 color;
    bool has_color = false;
    if (strstr(item, "[error]")) {
      color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
      has_color = true;
    } else if (strncmp(item, "# ", 2) == 0) {
      color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
      has_color = true;
    }
    if (has_color) ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextWrapped(item);
    if (has_color) ImGui::PopStyleColor();
  }

  if (copy_to_clipboard) ImGui::LogFinish();

  if (ScrollToBottom ||
      (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
    ImGui::SetScrollHereY(1.0f);

  ScrollToBottom = false;

  ImGui::PopStyleVar();
}

void ConsoleView::drawContextMenu() {
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::Selectable("Clear")) clearLog();
    copy_to_clipboard = ImGui::SmallButton("Copy");
    ImGui::Checkbox("Auto-scroll", &AutoScroll);
    ImGui::Checkbox("Debug Callback", &DebugCallback);
    ImGui::EndPopup();
  }
}

void ConsoleView::drawConsole() {
  // Console Display
  ImGui::BeginChild("ScrollingRegion",
                    ImVec2(0, ImGui::GetContentRegionAvail().y * 0.8f), false,
                    ImGuiWindowFlags_None);
  drawContextMenu();
  displayCommands();
  ImGui::EndChild();
  ImGui::Separator();

  // Input Widget
  if (ImGui::BeginTable("##consoleWidget", 3, ImGuiTableFlags_SizingStretchProp,
                        ImVec2(0, ImGui::GetContentRegionAvail().y * 0.2f))) {
    ImGui::TableSetupColumn("##first");
    ImGui::TableSetupColumn("##second");
    ImGui::TableSetupColumn("##third");
    ImGui::TableNextColumn();
    ImGui::Text(ICON_MD_TERMINAL);
    ImGui::SameLine();
    if (ImGui::InputText(
            "##consoleInput", InputBuf, IM_ARRAYSIZE(InputBuf),
            input_text_flags,
            [](ImGuiInputTextCallbackData* data) {
              auto console = (ConsoleView*)data->UserData;
              return console->TextEditCallback(data);
            },
            (void*)this)) {
      char* s = InputBuf;
      Strtrim(s);
      if (s[0]) executeCommand(s);
      strcpy(s, "");
      reclaim_focus = true;
    }

    ImGui::TableNextColumn();
    Filter.Draw("Filter", 75);
    ImGui::TableNextColumn();
    ImGui::SetItemDefaultFocus();  // Auto-focus on window apparition
    if (reclaim_focus)
      ImGui::SetKeyboardFocusHere(-1);  // Auto focus previous widget

    ImGui::EndTable();
  }
}

ConsoleView::ConsoleView() : View() {
  clearLog();
  memset(InputBuf, 0, sizeof(InputBuf));
  Commands.push_back("HELP");
  Commands.push_back("HISTORY");
  Commands.push_back("CLEAR");
  Commands.push_back(
      "CLASSIFY");  // "CLASSIFY" is here to provide the test case where
                    // "C"+[tab] completes to "CL" and display multiple matches.
  Commands.push_back("LOAD_QUOTE");
  Commands.push_back("CLOSE_SOCKET");
  addLogStd("Welcome to Premia!");
  addLogStd(
      "Enter 'HELP' for help. TAB key for autocomplete, UP/DOWN key for "
      "history");
}

ConsoleView::~ConsoleView() {
  clearLog();
  for (int i = 0; i < History.Size; i++) free(History[i]);
}

String ConsoleView::getName() { return "Console"; }

void ConsoleView::addLog(const char* fmt, ...) {
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
  buf[IM_ARRAYSIZE(buf) - 1] = 0;
  va_end(args);
  Items.push_back(Strdup(buf));
}

void ConsoleView::addLogStd(CRString data) {
  const boost::posix_time::ptime now =
      boost::posix_time::second_clock::local_time();
  auto hours = now.time_of_day().hours();
  auto minutes = now.time_of_day().minutes();

  String log = "[";
  log += std::to_string(hours);
  log += ":" + std::to_string(minutes) + "] " + data;

  Items.push_back(Strdup(log.c_str()));
}

void ConsoleView::addLogger(const Logger& newLogger) {
  this->logger = newLogger;
}

void ConsoleView::addEvent(CRString key, const EventHandler& event) {
  this->events[key] = event;
}

void ConsoleView::update() {
  if (guiDemo) ImGui::ShowDemoWindow();

  if (plotDemo) ImPlot::ShowDemoWindow();

  drawConsole();
}

}  // namespace premia