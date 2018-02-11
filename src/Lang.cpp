#include "ofApp.h"

void Lang::setup(){
    draw_entity = [](){};

    camera.setDistance(1);
    camera.setNearClip(0.01);
    camera.disableMouseInput();

    ofFbo master;
    fbo_map[":master"] = master;

    define_commands();
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
    ofEnableAlphaBlending();

    texture.draw((w - side) / 2,
                 (h - side) / 2,
                 side, side);

    ofDisableAlphaBlending();
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
    if (entity_stack.empty()) {
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
    if (symbol_stack.empty()) {
        console->log("[error] Missing symbol argument");
        throw exception();
    }
}

Entity Lang::pop_entity(){
    Entity entity = entity_stack.back();
    entity_stack.pop_back();
    return entity;
}

Numeric Lang::pop_numeric(){
    Numeric n = numeric_stack.back();
    numeric_stack.pop_back();
    return n;
}


string Lang::pop_sybmol(){
    string s = symbol_stack.back();
    symbol_stack.pop_back();
    return s;
}

void Lang::push_entity(Entity entity){
    entity_stack.push_back(entity);
}

void Lang::push_numeric(Numeric n){
    numeric_stack.push_back(n);
}

void Lang::push_symbol(string s){
    symbol_stack.push_back(s);
}

void Lang::add_command(string name, string example, string doc,
                       function<void(void)> action){
    Command cmd;
    cmd.doc = doc;
    cmd.example = example;
    cmd.action = action;
    commands[name] = cmd;
}

void Lang::add_command(string name, string example,
                       function<void(void)> action){
    add_command(name, example, "", action);
}

void Lang::add_command(string name,
                       function<void(void)> action){
    add_command(name, "", "", action);
}

void Lang::on_float(float x){
    push_numeric([=](){return x;});
}

void Lang::on_symbol(string s){
    push_symbol(s);
}

void Lang::on_definition(string s){
    string name = s.substr(1);

    vector<Entity>  es  = entity_stack;
    vector<Numeric> ns = numeric_stack;
    vector<string>  ss  = symbol_stack;

    entity_stack.clear();
    numeric_stack.clear();
    symbol_stack.clear();

    add_command(name, "", "", [=](){
        entity_stack.insert(entity_stack.end(), es.begin(), es.end());
        numeric_stack.insert(numeric_stack.end(), ns.begin(), ns.end());
        symbol_stack.insert(symbol_stack.end(), ss.begin(), ss.end());
    });

    console->log("[defined] %s", name.c_str());
}

void Lang::eval(char* command_line){
    char *token;
    token = strtok(command_line, " ,");

    while (token != NULL){
        exec_command(token);
        token = strtok(NULL, " ,");
    }
}

void Lang::exec_command(string name)
{
    if (commands.count(name) == 1){
        try{
            commands[name].action();
        } catch(exception e){
            console->log("[error] in command %s", name.c_str());
        }
    } else if (name[0] == ':'){
        on_symbol(name);
    } else if (name[0] == '@' && name.size() > 1){
        on_definition(name);
    } else{
        try{
            float x = stof(name, NULL);
            on_float(x);
        } catch (const std::invalid_argument& ia){
            // TODO stream logger
            ostringstream string_stream;
            string_stream << "Unknown command: '" << name << "'\n";
            console->log(string_stream.str().c_str());
            //ofLog() << "Unknown command: '" << name << "\n";
        }
    }
}
