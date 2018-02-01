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

Numeric ofApp::pop_numeric(){
    if (!numeric_stack.empty()) {
        Numeric n = numeric_stack.back();
        numeric_stack.pop_back();
        return n;
    } else{
        console.log("[error] Missing numeric argument");
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

void ofApp::push_entity(Entity entity){
    entities_stack.push_back(entity);
}

void ofApp::push_numeric(Numeric n){
    numeric_stack.push_back(n);
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

Slider::Slider(string name){
    x = 1;

    draw = [=](){
        ImGui::SliderFloat(name.c_str(), &(this->x), 0, 1, "%.2f");
    };

    get = [=](){
        return this->x;
    };
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
    camera.disableMouseInput();

    ofFbo master;
    fbo_map[":master"] = master;

    console.lang = &lang;
    lang.console = &console;  // TODO pass only a logger function

    lang.add_command("slider", "", [=](){
        try{
            string name = pop_sybmol();
            if (!sliders_numeric.count(name)){
                Slider *s = new Slider(name);  // on the heap
                sliders_gui.push_back(s->draw);
                sliders_numeric[name] = s->get;
            }
            push_numeric(sliders_numeric[name]);
        } catch(exception e){}
    });

    draw_entity = [](){};

    lang.on_float([=](float x){
        push_numeric([=](){return x;});
    });

    lang.on_symbol([=](string s){
        push_symbol(s);
    });

    lang.add_command("time", "", [=](){
        push_numeric([=](){
            return ofGetElapsedTimef();
        });
    });

    lang.add_command("sin", "", [=](){
        try{
            Numeric n = pop_numeric();
            push_numeric([=](){
                return sin(n());
            });
        } catch(exception e){}
    });

    lang.add_command("cube", "", [=](){
        push_entity([=](){
            ofDrawBox(0.5);
        });
    });

    lang.add_command("scale", " \n\t e x scale", [=](){
        try{
            Entity entity = pop_entity();
            Numeric n = pop_numeric();

            push_entity([=](){
                ofPushMatrix();
                ofScale(n(), n(), n());
                entity();
                ofPopMatrix();
            });
        } catch(exception e){}
    });

    lang.add_command("to", "draw to fbo \n\t e :a to", [=](){
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

    lang.add_command("from", "draw from fbo \n\t :a from", [=](){
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

    lang.add_command("]", "merge all the entities \n\t e1 e2 ... ]", [=](){
        while (entities_stack.size() > 1){
            Entity e1 = pop_entity();
            Entity e2 = pop_entity();

            push_entity([=](){
                e1();
                e2();
            });
        }
    });

    lang.add_command(".", "draw the last entity \n\t e .", [=](){
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


    for (size_t i = 0; i < sliders_gui.size(); i++){
        sliders_gui[i]();  // draw the slider
    }

    ImGui::End();

    gui.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 769){  // CTRL
        camera.enableMouseInput();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 769){  // CTRL
        camera.disableMouseInput();
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
