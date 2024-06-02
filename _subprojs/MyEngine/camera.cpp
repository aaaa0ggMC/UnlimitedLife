#include <com/camera.h>
#include <com/window.h>

using namespace me;
using namespace std;

///Camera
void Camera::BuildOrth(float l,float r,float b,float t,float n,float f){
    perspec = glm::ortho(l,r,b,t,n,f);
}

//View Mat,not model
void Camera::UpdateModelMat(){
    mat = glm::translate(glm::mat4(1.0),glm::vec3(-position.x,-position.y,-position.z));
    if(updateVRP)vrp_matrix = perspec * glm::transpose(rmat) * mat;
//    mat = mat * rmat;转到另一个
}

Camera::Camera(float x,float y,float z,bool m,bool uvrp){
    SetPosition(x,y,z);
    movement = m;
    SetRotation(0,0,0);
    updateVRP = uvrp;
}

void Camera::BuildPerspec(float fieldOfView,float ratio,float nearPlane,float farPlane){
    perspec = glm::perspective(fieldOfView,ratio,nearPlane,farPlane);
}

void Camera::BuildPerspec(float fieldOfView,float width,float height,float nearPlane,float farPlane){
    BuildPerspec(fieldOfView,width/height,nearPlane,farPlane);
}

void Camera::BuildPerspec(float fieldOfView,void*w,float nearPlane,float farPlane){
    glm::vec2 sz = ((Window*)(w))->GetBufferSize();
    BuildPerspec(fieldOfView,sz.x / sz.y,nearPlane,farPlane);
}

void Camera::BuildOrthA(float left,float top,float xlen,void*w,float znear,float zfar){
    glm::vec2 sz = ((Window*)(w))->GetBufferSize();
    BuildOrth(left,left + xlen,top + xlen * sz.y / sz.x,top,znear,zfar);
}
void Camera::BuildOrthA(float left,float top,float xlen,float aspectRatio,float znear,float zfar){
    BuildOrth(left,left + xlen,top + xlen / aspectRatio,top,znear,zfar);
}
