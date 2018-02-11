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

class Console;

class Slider{
    public:
        Slider();
        Numeric value;
        float *get_ptr();

    private:
        float x;
};

struct Command{
        string doc;
        string example;
        function<void(void)> action;
};

class Lang{
    public:
        void setup();
        void draw();

        void define_commands();

        void add_command(string name, string example, string doc,
                         function<void(void)> action);

        void add_command(string name, string doc,
                         function<void(void)> action);

        void add_command(string name,
                         function<void(void)> action);

        void check_entity();
        void check_numeric();
        void check_sybmol();

        Entity pop_entity();
        Numeric pop_numeric();
        string pop_sybmol();

        void push_entity(Entity entity);
        void push_numeric(Numeric n);
        void push_symbol(string s);

        map<string, Command> commands;

        void on_float(float x);
        void on_symbol(string s);
        void on_definition(string s);

        void eval(char* line);
        void exec_command(string command_line);

        Console *console;

        void on_fbo(string name, Entity e);

        vector<Entity> entity_stack;
        vector<Numeric>  numeric_stack;
        vector<string> symbol_stack;

        Entity draw_entity;
        map<string, Slider*> sliders;
        map<string, ofFbo> fbo_map;

        ofEasyCam camera;
};

class Console{
    public:
        Console();
        ~Console();

        void log(const char* fmt, ...);
        void clear_log();
        void draw();

        Lang *lang;  // TODO use callbacks when possible

    private:
        int TextEditCallback(ImGuiTextEditCallbackData* data);
        char                  InputBuf[256];
        ImVector<char*>       Items;
        bool                  ScrollToBottom;
        ImVector<char*>       history;
        int                   history_pos;  // -1: new line
};

class ofApp : public ofBaseApp{
    public:
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
        Lang lang;
};
