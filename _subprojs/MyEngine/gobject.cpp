#include <com/gobject.h>

using namespace me;

GObject::GObject(float x,float y,float z,bool m){
    position.x = x;
    position.y = y;
    position.z = z;
    scale = glm::vec3(1,1,1);
    movement = m;
    SetRotation(0,0,0);
    UpdateRotationMat();
    UpdateModelMat();
    vbo = coord = VBO(0);
    vbind = cbind = 0;
}

void GObject::Update(unsigned int){
    if(Match(ME_GROT)){
        UpdateRotationMat();
    }
    UpdateModelMat();
}

void GObject::SetPosition(float x,float y,float z){
    MarkDirty(ME_GMOD);
    position.x = x;
    position.y = y;
    position.z = z;
}


void GObject::SetBindings(unsigned int vb,unsigned int cb){
    vbind = vb;
    cbind = cb;
}

void GObject::SetPosition(glm::vec3 & v){
    MarkDirty(ME_GMOD);
    position.x = v.x;
    position.y = v.y;
    position.z = v.z;
}

glm::vec3 GObject::GetPosition(){return position;}
void GObject::Move(float x,float y,float z){
    MarkDirty(ME_GMOD);
    position.x += x;
    position.y += y;
    position.z += z;
}

void GObject::Move(glm::vec3 & v){
    MarkDirty(ME_GMOD);
    position.x += v.x;
    position.y += v.y;
    position.z += v.z;
}

glm::mat4 * GObject::GetMat(){return &mat;}

void GObject::UpdateModelMat(){
    mat = glm::scale(glm::mat4(1.0),scale);
    mat = glm::translate(mat,position);
    mat = mat * rmat;
}

void GObject::SetMovement(bool v){
    MarkDirty(ME_GROT);
    movement = v;
    Rotate(0,0,0);
}

void GObject::MoveDirectional(float l,float u,float f){
    if(!movement){
        ///处理错误，不需要很节省
        ME_SIV("Using MoveDirectional without setting the movement field!",0);
        return;
    }
    MarkDirty(ME_GROT);
    float x = l * left.x + u * up.x + f * forward.x;
    float y = l * left.y + u * up.y + f * forward.y;
    float z = l * left.z + u * up.z + f * forward.z;
    Move(x,y,z);
}
void GObject::BuildMV(glm::mat4 * m){
    mvmat = *m * mat;
}

void GObject::BuildMV(GObject * v){
    BuildMV(&(v->mat));
}

VBO GObject::GetVBO(){return vbo;}

void GObject::Rotate(float x,float y,float z){
    MarkDirty(ME_GROT);
    rotations += glm::vec3(x,y,z);
}

void GObject::SetRotation(float x,float y,float z){
    MarkDirty(ME_GROT);
    rotations = glm::vec3(x,y,z);
}

void GObject::UpdateRotationMat(){
    rmat = glm::rotate(glm::mat4(1.0),rotations.y,glm::vec3(0.0,1.0,0.0));
    rmat = glm::rotate(rmat,rotations.x,glm::vec3(1.0,0.0,0.0));
    rmat = glm::rotate(rmat,rotations.z,glm::vec3(0.0,0.0,1.0));
    left = rmat * glm::vec4(1,0,0,1);
    up = rmat * glm::vec4(0,1,0,1);
    forward = rmat * glm::vec4(0,0,-1,1);
}

void GObject::BindVBO(VBO invbo,VBO vx){
    this->vbo = invbo;
    this->coord = vx;
}

void GObject::Scale(float x,float y,float z){
    MarkDirty(ME_GMOD);
    scale.x *= x;
    scale.y *= y;
    scale.z *= z;
}

void GObject::SetScale(float x,float y,float z){
    MarkDirty(ME_GMOD);
    scale.x = x;
    scale.y = y;
    scale.z = z;
}
