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

string ofApp::pop_sybmol(){
    if (!symbols_stack.empty()) {
        string s = symbols_stack.back();
        symbols_stack.pop_back();
        return s;
    } else{
        console.log("[error] Missing symbol argument");
        throw exception();
    }
}

float ofApp::pop_float(){
    if (!float_stack.empty()) {
        float x = float_stack.back();
        float_stack.pop_back();
        return x;
    } else{
        console.log("[error] Missing number argument");
        throw exception();
    }
}

void ofApp::push_entity(Entity entity){
    entities_stack.push_back(entity);
}

void ofApp::push_float(float x){
    float_stack.push_back(x);
}

void ofApp::push_symbol(string s){
    symbols_stack.push_back(s);
}

void ofApp::on_fbo(string name, Entity e){
    fbo_map[name].begin();

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA,
                        GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE,
                        GL_ONE_MINUS_SRC_ALPHA);

    ofClear(255,255,255, 0);
    ofSetColor(255);
    ofNoFill();
    ofSetLineWidth(2);

    e();

    glDisable(GL_BLEND);
    glPopAttrib();
    fbo_map[name].end();
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

    ofFbo master;
    fbo_map[":master"] = master;

    draw_entity = [](){};

    console.on_float([=](float x){
        push_float(x);
    });

    console.on_symbol([=](string s){
        push_symbol(s);
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

    console.add_command("to", [=](){
        try{
            Entity entity = pop_entity();
            string name = pop_sybmol();
            if (fbo_map.count(name) == 0){
                ofFbo layer;
                fbo_map[name] = layer;
                fbo_map[name].allocate(BUFFER_SIZE, BUFFER_SIZE, GL_RGBA);
            }

            push_entity([=](){
                on_fbo(name, [=](){
                    static ofEasyCam camera;
                    //camera.disableMouseInput();
                    camera.setDistance(1);
                    camera.setNearClip(0.01);

                    camera.begin();
                    entity();
                    camera.end();
                });
            });
        } catch(exception e){}
    });

    console.add_command("from", [=](){
        try{
            string name = pop_sybmol();
            push_entity([=](){
                ofEnableAlphaBlending();
                if (fbo_map.count(name) != 0){
                    fbo_map[name].getTexture().draw(-0.5, -0.5, 1, 1);
                }
                ofDisableAlphaBlending();
            });
        } catch(exception e){}
    });

    console.add_command("]", [=](){
        try{
            Entity e1 = pop_entity();
            Entity e2 = pop_entity();

            push_entity([=](){
                e1();
                e2();
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
    fbo_map[":master"].allocate(BUFFER_SIZE, BUFFER_SIZE, GL_RGBA);

    on_fbo(":master", [=](){
        ofBackground(0, 0, 0);
        camera.begin();
        draw_entity();
        camera.end();
    });

    int w = ofGetWidth();
    int h = ofGetHeight();
    int side = max(w, h);

    ofTexture texture = fbo_map[":master"].getTexture();
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
