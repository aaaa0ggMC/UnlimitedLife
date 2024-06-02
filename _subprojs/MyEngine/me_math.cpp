#include <com/me_math.h>

using namespace me;

Velocity::Velocity(float v){
    vv = glm::vec3(0,0,0);
    SetVelocity(v);
}

void Velocity::New(){
    vv.x = vv.y = vv.z = 0;
}

void Velocity::Add(int x,int y,int z){
    vv.x += x;
    vv.y += y;
    vv.z += z;
}

void Velocity::Form(){
    float len = glm::length(vv);
    if(len)vv = v / len * vv;
}

void Velocity::MoveDr(GObject & g,float et){
    g.MoveDirectional(vv.x * et,vv.y * et,vv.z * et);
}

void Velocity::Move(GObject & g,float et){
    g.Move(vv.x * et,vv.y * et,vv.z * et);
}

void Velocity::SetVelocity(float cc){v = cc;}
