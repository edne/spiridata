#include "ofApp.h"
#include <ctype.h>  // toupper, isprint
#include <strings.h>
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

Console::Console(){
    clear_log();
    memset(InputBuf, 0, sizeof(InputBuf));
    history_pos = -1;

    /*
    add_command("help", [=](){
        log("Commands:");
        for (size_t i = 0; i < commands_names.size(); i++)
            log("- %s", commands_names[i]);
    });
    */
    on_float_cb = [](float x){};
}

Console::~Console(){
    clear_log();
    for (int i = 0; i < history.Size; i++)
        free(history[i]);
}

void Console::clear_log(){
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);
    Items.clear();
    ScrollToBottom = true;
}

void Console::log(const char* fmt, ...){
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf)-1] = 0;
    va_end(args);
    Items.push_back(strdup(buf));
    ScrollToBottom = true;
}

void Console::add_command(const char* name, function<void(void)> action){
    commands_names.push_back(name);
    commands_map[name] = action;
}

void Console::on_float(function<void(float)> f){
    on_float_cb = f;
}

void Console::draw(){
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
            char* command_line = InputBuf;
            log("%s", command_line);
            history.push_back(strdup(command_line));
            history_pos = -1;

            char *token;
            token = strtok(command_line, " ");

            while (token != NULL){
                exec_command(token);
                token = strtok(NULL, " ");
            }
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

void Console::exec_command(const char* name)
{
    if (commands_map.count(name) == 1){
        commands_map[name]();
    } else{
        try{
            float x = stof(name, NULL);
            on_float_cb(x);
        } catch (const std::invalid_argument& ia){
            log("Unknown command: '%s'\n", name);
        }
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
            for (size_t i = 0; i < commands_names.size(); i++)
                if (strncmp(commands_names[i], word_start,
                            (int)(word_end-word_start)) == 0)
                    candidates.push_back(commands_names[i]);

            if (candidates.Size == 0){
                log("No match for \"%.*s\"!\n",
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
                log("Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    log("- %s\n", candidates[i]);
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:{
            const int prev_history_pos = history_pos;
            if (data->EventKey == ImGuiKey_UpArrow){
                if (history_pos == -1)
                    history_pos = history.Size - 1;
                else if (history_pos > 0)
                    history_pos--;
            }
            else if (data->EventKey == ImGuiKey_DownArrow){
                if (history_pos != -1)
                    if (++history_pos >= history.Size)
                        history_pos = -1;
            }
            if (prev_history_pos != history_pos){
                int pos = snprintf(data->Buf, (size_t)data->BufSize,
                                   "%s", (history_pos >= 0) ? history[history_pos] : "");
                data->CursorPos = data->SelectionStart =
                                  data->SelectionEnd =
                                  data->BufTextLen = pos;
                data->BufDirty = true;
            }
        }
    }
    return 0;
}
