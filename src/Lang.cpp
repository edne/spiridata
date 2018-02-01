#include "ofApp.h"

Lang::Lang(){
    on_float_cb = [](float x){};
    on_symbol_cb = [](string s){};
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
        commands_map[name]();
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
