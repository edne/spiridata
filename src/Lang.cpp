#include "ofApp.h"

void Lang::setup(){
    on_float_cb = [](float x){};
    on_symbol_cb = [](string s){};

    draw_entity = [](){};

    camera.setDistance(1);
    camera.setNearClip(0.01);
    camera.disableMouseInput();

    ofFbo master;
    fbo_map[":master"] = master;

    add_command("slider", "", [=](){
        check_sybmol();
        string name = pop_sybmol();

        if (sliders.count(name) == 0){
            Slider *s = new Slider();  // on the heap
            sliders[name] = s;
        }
        push_numeric(sliders[name]->value);
    });

    on_float([=](float x){
        push_numeric([=](){return x;});
    });

    on_symbol([=](string s){
        push_symbol(s);
    });

    add_command("time", "", [=](){
        push_numeric([=](){
            return ofGetElapsedTimef();
        });
    });

    add_command("sin", "", [=](){
        check_numeric();
        Numeric n = pop_numeric();
        push_numeric([=](){
            return sin(n());
        });
    });

    add_command("cube", "", [=](){
        push_entity([=](){
            ofDrawBox(0.5);
        });
    });

    add_command("scale", " \n\t e x scale", [=](){
        check_entity();
        check_numeric();

        Entity entity = pop_entity();
        Numeric n = pop_numeric();

        push_entity([=](){
            ofPushMatrix();
            ofScale(n(), n(), n());
            entity();
            ofPopMatrix();
        });
    });

    add_command("to", "draw to fbo \n\t e :a to", [=](){
        check_entity();
        check_sybmol();

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
                camera.disableMouseInput();
                camera.setDistance(1);
                camera.setNearClip(0.01);

                camera.begin();
                entity();
                camera.end();
            });
        });
    });

    add_command("from", "draw from fbo \n\t :a from", [=](){
        check_sybmol();
        string name = pop_sybmol();

        push_entity([=](){
            ofEnableAlphaBlending();
            if (fbo_map.count(name) != 0){
                fbo_map[name].getTexture().draw(-0.5, -0.5, 1, 1);
            }
            ofDisableAlphaBlending();
        });
    });

    add_command("]", "merge all the entities \n\t e1 e2 ... ]", [=](){
        while (entities_stack.size() > 1){
            Entity e1 = pop_entity();
            Entity e2 = pop_entity();

            push_entity([=](){
                e1();
                e2();
            });
        }
    });

    add_command(".", "draw the last entity \n\t e .", [=](){
        check_entity();
        draw_entity = pop_entity();
    });

}

void Lang::draw(){
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
}

void Lang::on_fbo(string name, Entity e){
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

void Lang::check_entity(){
    if (entities_stack.empty()) {
        console->log("[error] Missing entity argument");
        throw exception();
    }
}

void Lang::check_numeric(){
    if (numeric_stack.empty()) {
        console->log("[error] Missing numeric argument");
        throw exception();
    }
}

void Lang::check_sybmol(){
    if (symbols_stack.empty()) {
        console->log("[error] Missing symbol argument");
        throw exception();
    }
}

Entity Lang::pop_entity(){
    Entity entity = entities_stack.back();
    entities_stack.pop_back();
    return entity;
}

Numeric Lang::pop_numeric(){
    Numeric n = numeric_stack.back();
    numeric_stack.pop_back();
    return n;
}


string Lang::pop_sybmol(){
    string s = symbols_stack.back();
    symbols_stack.pop_back();
    return s;
}

void Lang::push_entity(Entity entity){
    entities_stack.push_back(entity);
}

void Lang::push_numeric(Numeric n){
    numeric_stack.push_back(n);
}

void Lang::push_symbol(string s){
    symbols_stack.push_back(s);
}



void Lang::add_command(const char *name,
                          const char *doc,
                          function<void(void)> action){
    commands_names.push_back(name);
    commands_map[name] = action;
    commands_doc[name] = doc;
}

void Lang::on_float(function<void(float)> f){
    on_float_cb = f;
}

void Lang::on_symbol(function<void(string)> f){
    on_symbol_cb = f;
}

void Lang::eval(char* command_line){
    char *token;
    token = strtok(command_line, " ,");

    while (token != NULL){
        exec_command(token);
        token = strtok(NULL, " ,");
    }
}

void Lang::exec_command(const char* name)
{
    if (commands_map.count(name) == 1){
        try{
            commands_map[name]();
        } catch(exception e){}
    } else if (name[0] == ':'){
        on_symbol_cb(name);
    } else{
        try{
            float x = stof(name, NULL);
            on_float_cb(x);
        } catch (const std::invalid_argument& ia){
            // TODO stream logger
            ostringstream string_stream;
            string_stream << "Unknown command: '" << name << "'\n";
            console->log(string_stream.str().c_str());
            //ofLog() << "Unknown command: '" << name << "\n";
        }
    }
}
