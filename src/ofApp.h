#pragma once

#include "ofMain.h"
#include "ofxImGui.h"

#include <vector>
#include <map>
#include <functional>
#include <string>     // std::string, std::stof
#include <stdexcept>  // std::invalid_argument

using namespace std;

typedef function<void(void)> Entity;

const int BUFFER_SIZE = 1366;

class Console{
    public:
        Console();
        ~Console();

        void log(const char* fmt, ...);
        void clear_log();
        void draw();

        void add_command(const char* name, function<void(void)> action);
        void on_float(function<void(float)>);

    private:
        void exec_command(const char* command_line);

        int TextEditCallback(ImGuiTextEditCallbackData* data);
        char                  InputBuf[256];
        ImVector<char*>       Items;
        bool                  ScrollToBottom;
        ImVector<char*>       history;
        int                   history_pos;  // -1: new line

        vector<const char*> commands_names;
        map<string, function<void(void)>> commands_map;
        function<void(float)> on_float_cb;
};

class ofApp : public ofBaseApp{
    public:
        Entity pop_entity();
        float pop_float();
        void push_entity(Entity entity);
        void push_float(float x);

        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y);
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseEntered(int x, int y);
        void mouseExited(int x, int y);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);

    private:
        ofxImGui::Gui gui;

        Console console;
        Entity draw_entity;
        vector<Entity> entities_stack;
        vector<float>  float_stack;

        ofEasyCam camera;
        ofFbo output;
};
