#include "ofApp.h"
#include <ctype.h>  // toupper, isprint
#include <strings.h>
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

Console::Console(){
    ClearLog();
    memset(InputBuf, 0, sizeof(InputBuf));
    HistoryPos = -1;
    Commands.push_back("HELP");
    Commands.push_back("HISTORY");
    Commands.push_back("CLEAR");
    Commands.push_back("CLASSIFY");
}

Console::~Console(){
    ClearLog();
    for (int i = 0; i < History.Size; i++)
        free(History[i]);
}

void Console::ClearLog(){
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);
    Items.clear();
    ScrollToBottom = true;
}

void Console::AddLog(const char* fmt, ...){
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf)-1] = 0;
    va_end(args);
    Items.push_back(strdup(buf));
    ScrollToBottom = true;
}

void Console::Draw(){
    ImGui::BeginChild("ScrollingRegion",
                      ImVec2(0,-ImGui::GetItemsLineHeightWithSpacing()),
                      false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    // Tighten spacing
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); 

    for (int i = 0; i < Items.Size; i++)
        ImGui::TextUnformatted(Items[i]);

    if (ScrollToBottom)
        ImGui::SetScrollHere();
    ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    if (ImGui::InputText(" ", InputBuf,
                         IM_ARRAYSIZE(InputBuf),
                         ImGuiInputTextFlags_EnterReturnsTrue |
                         ImGuiInputTextFlags_CallbackCompletion |
                         ImGuiInputTextFlags_CallbackHistory,
                         [](ImGuiTextEditCallbackData* data){
                             Console* console = (Console*)data->UserData;
                             return console->TextEditCallback(data);
                         },
                        (void*)this)){

        char* input_end = InputBuf + strlen(InputBuf);
        while (input_end > InputBuf && input_end[-1] == ' ')
            input_end--;
        *input_end = 0;

        if (InputBuf[0]){
            const char* command_line = InputBuf;
            AddLog("# %s\n", command_line);

            ExecCommand(InputBuf);
        }

        strcpy(InputBuf, "");
    }

    // Keeping auto focus on the input box
    if (ImGui::IsItemHovered() ||
        (ImGui::IsRootWindowOrAnyChildFocused() &&
         !ImGui::IsAnyItemActive() &&
         !ImGui::IsMouseClicked(0)))
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
}

void Console::ExecCommand(const char* command_line)
{
    if (strcmp(command_line, "CLEAR") == 0){
        ClearLog();
    }
    else if (strcmp(command_line, "HELP") == 0){
        AddLog("Commands:");
        for (int i = 0; i < Commands.Size; i++)
            AddLog("- %s", Commands[i]);
    }
    else if (strcmp(command_line, "HISTORY") == 0){
        for (int i = History.Size >= 10 ? History.Size - 10 : 0; i < History.Size; i++)
            AddLog("%3d: %s\n", i, History[i]);
    }
    else{
        AddLog("Unknown command: '%s'\n", command_line);
    }
}

int Console::TextEditCallback(ImGuiTextEditCallbackData* data){
    switch (data->EventFlag)
    {
        case ImGuiInputTextFlags_CallbackCompletion:{
            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf){
                const char c = word_start[-1];

                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < Commands.Size; i++)
                if (strncmp(Commands[i], word_start,
                            (int)(word_end-word_start)) == 0)
                    candidates.push_back(Commands[i]);

            if (candidates.Size == 0){
                AddLog("No match for \"%.*s\"!\n",
                       (int)(word_end-word_start), word_start);
            }
            else if (candidates.Size == 1){
                // Single match. Delete the beginning of the word and
                // replace it entirely so we've got nice casing
                data->DeleteChars((int)(word_start-data->Buf),
                                  (int)(word_end-word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else{
                // Multiple matches, complete as much as we can
                int match_len = (int)(word_end - word_start);
                for (;;) {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0;
                         i < candidates.Size && all_candidates_matches;
                         i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0){
                    data->DeleteChars((int)(word_start - data->Buf),
                                      (int)(word_end-word_start));
                    data->InsertChars(data->CursorPos,
                                      candidates[0],
                                      candidates[0] + match_len);
                }

                // List matches
                AddLog("Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    AddLog("- %s\n", candidates[i]);
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:{
            const int prev_history_pos = HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow){
                if (HistoryPos == -1)
                    HistoryPos = History.Size - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            }
            else if (data->EventKey == ImGuiKey_DownArrow){
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }
            if (prev_history_pos != HistoryPos){
                int pos = snprintf(data->Buf, (size_t)data->BufSize,
                                   "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
                data->CursorPos = data->SelectionStart =
                                  data->SelectionEnd =
                                  data->BufTextLen = pos;
                data->BufDirty = true;
            }
        }
    }
    return 0;
}
