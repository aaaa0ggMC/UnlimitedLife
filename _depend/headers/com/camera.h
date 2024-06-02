#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "gobject.h"
#include <vector>
#include "macros.h"
#include "utility.h"

namespace me{
    using namespace std;
    class Camera : public GObject{
    public:
        glm::mat4 perspec;
        glm::mat4 vrp_matrix;
        bool updateVRP;

        Camera(float x = 0,float y = 0,float z = 0,bool = true,bool = true);
        void UpdateModelMat();
        void BuildPerspec(float fieldOfView,float ratio,float nearPlane,float farPlane);
        void BuildPerspec(float fieldOfView,float width,float height,float nearPlane,float farPlane);
        void BuildPerspec(float fieldOfView,void*w,float nearPlane,float farPlane);
        void BuildOrth(float left,float right,float bottom,float top,float znear = 0.1,float zfar = 1000);
        void BuildOrthA(float left,float top,float xlen,void*w,float znear = 0.1,float zfar = 1000);
        void BuildOrthA(float left,float top,float xlen,float aspectRatio,float znear = 0.1,float zfar = 1000);
    };
}

#endif // CAMERA_H_INCLUDED
