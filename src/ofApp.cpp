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

    draw_entity = [](){};

    console.on_float([=](float x){
        float_stack.push_back(x);
    });

    console.add_command("empty", [=](){
        entities_stack.push_back([=](){});
    });

    console.add_command("cube", [=](){
        entities_stack.push_back([=](){
            ofDrawBox(0.5);
        });
    });

    console.add_command("scale", [=](){
        if (!entities_stack.empty()) {
            if (!float_stack.empty()) {
                function<void(void)> entity = entities_stack.back();
                float x = float_stack.back();
                entities_stack.pop_back();
                float_stack.pop_back();

                entities_stack.push_back([=](){
                    ofPushMatrix();
                    ofScale(x, x, x);
                    entity();
                    ofPopMatrix();
                });
            } else{
                console.log("[error] Missing float argument\n");
            }
        } else{
            console.log("[error] Missing entity to scale\n");
        }
    });

    console.add_command(".", [=](){
        if (!entities_stack.empty()) {
            draw_entity = entities_stack.back();
            entities_stack.pop_back();
        }
    });
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

    //ofDrawBox(0.5);
    draw_entity();

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

    console.draw();

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
