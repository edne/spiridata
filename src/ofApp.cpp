#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofLog() << "Running setup()";

    ofSetEscapeQuitsApp(false);
    gui.setup();

    ofSetVerticalSync(true);
    ofEnableDepthTest();
    ofEnableSmoothing();

    camera.setDistance(1);
    camera.setNearClip(0.01);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    output.allocate(BUFFER_SIZE, BUFFER_SIZE, GL_RGBA);

    output.begin();
    camera.begin();

    ofSetLineWidth(2);
    ofBackground(0, 0, 0);

    ofSetColor(255);
    ofNoFill();

    ofDrawBox(0.5);

    camera.end();
    output.end();

    int w = ofGetWidth();
    int h = ofGetHeight();
    int side = max(w, h);

    ofTexture texture = output.getTexture();
    texture.draw((w - side) / 2,
                 (h - side) / 2,
                 side, side);

    gui.begin();
    ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("", NULL,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_ShowBorders);

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1, 0.1, 0.1, 0.25);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.1, 0.1, 0.1, 0.25);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1, 0.1, 0.1, 0.25);

    console.Draw();

    ImGui::SameLine();
    ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);

    ImGui::End();
    gui.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
