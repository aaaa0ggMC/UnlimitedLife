#include "com/program.h"

using namespace me;

void Program::PushObj(std::vector<GObject*> l){
    for(auto a : l){
        objs.push_back(a);
    }
}

void Program::Update(){
    for(GObject*o : objs){
        o->Update();
    }
}
