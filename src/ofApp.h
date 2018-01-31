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
typedef function<float(void)> Numeric;

const int BUFFER_SIZE = 1366;

class Console{
    public:
        Console();
        ~Console();

        void log(const char* fmt, ...);
        void clear_log();
        void draw();

        void add_command(const char *name,
                         const char *doc,
                         function<void(void)> action);
        void on_float(function<void(float)>);
        void on_symbol(function<void(string)>);

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
        map<string, const char*> commands_doc;

        function<void(float)> on_float_cb;
        function<void(string)> on_symbol_cb;
};

class Slider{
    public:
        Slider(string name);
        function<void(void)> draw;
        function<float(void)> get;

    private:
        float x;
};

class ofApp : public ofBaseApp{
    public:
        Entity pop_entity();
        Numeric pop_numeric();
        string pop_sybmol();

        void push_entity(Entity entity);
        void push_numeric(Numeric n);
        void push_symbol(string s);

        void on_fbo(string name, Entity e);
        void draw_fbo(string name);

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
        vector<Numeric>  numeric_stack;
        vector<string> symbols_stack;

        map<string, ofFbo> fbo_map;

        map<string, Numeric> sliders_numeric;      // getters
        vector<function<void(void)>> sliders_gui;  // draw functions


        ofEasyCam camera;
};
