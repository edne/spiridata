#include "ofApp.h"

Entity ofApp::pop_entity(){
    if (!entities_stack.empty()) {
        Entity entity = entities_stack.back();
        entities_stack.pop_back();
        return entity;
    } else{
        console.log("[error] Missing entity argument");
        throw exception();
    }
}

float ofApp::pop_float(){
    if (!float_stack.empty()) {
        float x = float_stack.back();
        float_stack.pop_back();
        return x;
    } else{
        console.log("[error] Missing float argument");
        throw exception();
    }
}

void ofApp::push_entity(Entity entity){
    entities_stack.push_back(entity);
}

void ofApp::push_float(float x){
    float_stack.push_back(x);
}

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
        push_float(x);
    });

    console.add_command("empty", [=](){
        push_entity([=](){});
    });

    console.add_command("cube", [=](){
        push_entity([=](){
            ofDrawBox(0.5);
        });
    });

    console.add_command("scale", [=](){
        try{
            Entity entity = pop_entity();
            float x = pop_float();

            push_entity([=](){
                ofPushMatrix();
                ofScale(x, x, x);
                entity();
                ofPopMatrix();
            });
        } catch(exception e){}
    });

    console.add_command(".", [=](){
        try{
            draw_entity = pop_entity();
        } catch(exception e){}
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
