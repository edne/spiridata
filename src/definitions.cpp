#include "ofApp.h"

void Lang::def_unary_numeric(string name, function<float(float)> f){
    add_command(name, [=](){
        check_numeric();
        Numeric n = pop_numeric();
        push_numeric([=](){ return f(n()); });
    });
}

void Lang::def_binary_numeric(string name, function<float(float, float)> f){
    add_command(name, [=](){
        check_numeric();
        check_numeric();
        Numeric n = pop_numeric();
        Numeric m = pop_numeric();
        push_numeric([=](){ return f(n(), m()); });
    });
}

void Lang::define_commands(){
    def_unary_numeric("sin", [](float x){ return sin(x); });
    def_unary_numeric("cos", [](float x){ return cos(x); });
    def_unary_numeric("exp", [](float x){ return exp(x); });
    def_unary_numeric("log", [](float x){ return log(x); });
    def_unary_numeric("abs", [](float x){ return fabs(x); });

    def_binary_numeric("+", [](float x, float y){ return y+x; });
    def_binary_numeric("-", [](float x, float y){ return y-x; });
    def_binary_numeric("*", [](float x, float y){ return y*x; });
    def_binary_numeric("/", [](float x, float y){ return y/x; });
    def_binary_numeric("%", [](float x, float y){ return fmod(y, x); });


    add_command("slider", ":a slider", [=](){
        check_sybmol();
        string name = pop_sybmol();

        if (sliders.count(name) == 0){
            Slider *s = new Slider();  // on the heap
            sliders[name] = s;
        }
        push_numeric(sliders[name]->value);
    });

    add_command("time", [=](){
        push_numeric([=](){
            return ofGetElapsedTimef();
        });
    });

    add_command("polygon", "x polygon", [=](){
        check_numeric();
        Numeric n = pop_numeric();

        push_entity([=](){
        ofPushMatrix();

        ofRotateZ(90);

        ofSetCircleResolution(n());
        ofDrawCircle(0, 0, 0.5);

        ofPopMatrix();
        });
    });

    add_command("square", [=](){
        push_numeric([=](){return 4;});
        exec_command("polygon");
    });

    add_command("cube", [=](){
        push_entity([=](){
            ofDrawBox(0.5);
        });
    });

    add_command("scale", "e x scale", [=](){
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

    add_command("rotate", "e x rotate", "angle in 0-1 range", [=](){
        check_entity();
        check_numeric();

        Entity entity = pop_entity();
        Numeric n = pop_numeric();

        push_entity([=](){
            ofPushMatrix();
            ofRotate(n() * 360, 0, 0, 1);
            entity();
            ofPopMatrix();
        });
    });

    add_command("to", "e :a to", "draw to frame buffer", [=](){
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

    add_command("from", ":a from", "draw from framebuffer", [=](){
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

    add_command("]", "e1 e2 ... ]", "merge all the entities", [=](){
        while (entity_stack.size() > 1){
            Entity e1 = pop_entity();
            Entity e2 = pop_entity();

            push_entity([=](){
                e1();
                e2();
            });
        }
    });

    add_command(".", "e1 e2 ... .", "draw entities", [=](){
        check_entity();
        exec_command("]");
        draw_entity = pop_entity();
    });
}
