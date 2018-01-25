#pragma once

#include "ofMain.h"
#include "ofxImGui.h"

const int BUFFER_SIZE = 1366;

class Console{
    public:
        Console();
        ~Console();

        void ClearLog();
        void AddLog(const char* fmt, ...);
        void Draw();
        void ExecCommand(const char* command_line);

        int TextEditCallback(ImGuiTextEditCallbackData* data);
        char                  InputBuf[256];
        ImVector<char*>       Items;
        bool                  ScrollToBottom;
        ImVector<char*>       History;
        int                   HistoryPos;  // -1: new line
        ImVector<const char*> Commands;
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
        ofEasyCam camera;
        ofFbo output;
};
