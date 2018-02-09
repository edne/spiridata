#include "ofApp.h"

void Lang::define_commands(){
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

    add_command("sin", "x sin", [=](){
        check_numeric();
        Numeric n = pop_numeric();
        push_numeric([=](){
            return sin(n());
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
        while (entities_stack.size() > 1){
            Entity e1 = pop_entity();
            Entity e2 = pop_entity();

            push_entity([=](){
                e1();
                e2();
            });
        }
    });

    add_command(".", "e .", "draw the last entity", [=](){
        check_entity();
        draw_entity = pop_entity();
    });
}
