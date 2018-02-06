#include "ofApp.h"

Slider::Slider(){
    x = 1;

    value = [=](){
        return this->x;
    };
}

float *Slider::get_ptr(){
    return &(this->x);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofLog() << "Running setup()";

    ofSetEscapeQuitsApp(false);
    gui.setup();

    ofSetVerticalSync(true);
    ofEnableDepthTest();
    ofEnableSmoothing();

    console.lang = &lang;
    lang.console = &console;  // TODO just only a logger function

    lang.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    lang.draw();

    gui.begin();

    ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Console", NULL,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_ShowBorders);

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1, 0.1, 0.1, 0.25);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.1, 0.1, 0.1, 0.25);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1, 0.1, 0.1, 0.25);

    console.draw();

    ImGui::SameLine();
    ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);

    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Sliders", NULL,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_ShowBorders);

    for (auto const& [name, s] : lang.sliders){
        ImGui::SliderFloat(name.c_str(), s->get_ptr(), 0, 1, "%.2f");
    }

    ImGui::End();

    gui.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 769){  // CTRL
        lang.camera.enableMouseInput();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 769){  // CTRL
        lang.camera.disableMouseInput();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
