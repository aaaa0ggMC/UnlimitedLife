#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED
#include "gobject.h"
#include <vector>
#include "macros.h"
#include "utility.h"

#define ME_CAMERA_BUILD_PERSPEC 0
#define ME_CAMERA_BUILD_ORTH 1

namespace me{
    using namespace std;
    class Camera : public GObject{
    public:
        glm::mat4 perspec;
        glm::mat4 vrp_matrix;
        bool updateVRP;

        unsigned int buildMethod;

        ///perspec
        float fov;
        float nearPlane;
        float farPlane;

        ///orth
        float left;
        float right;
        float bottom;
        float top;
        float znear;
        float zfar;


        void SetPerspec(float fovRad,float nearPlane=0.1,float farPlane=1000);
        void SetOrtho(float left,float right,float bottom,float top,float znear = 0.1,float zfar = 1000);

        Camera(float x = 0,float y = 0,float z = 0,bool = true,bool = true);
        void UpdateModelMat();
        void BuildPerspec(float fieldOfView,float ratio,float nearPlane,float farPlane);
        void BuildPerspec(float fieldOfView,float width,float height,float nearPlane,float farPlane);
        void BuildPerspec(float fieldOfView,void*w,float nearPlane,float farPlane);
        void BuildOrth(float left,float right,float bottom,float top,float znear = 0.1,float zfar = 1000);
        void BuildOrthA(float left,float top,float xlen,void*w,float znear = 0.1,float zfar = 1000);
        void BuildOrthA(float left,float top,float xlen,float aspectRatio,float znear = 0.1,float zfar = 1000);

        void BuildPerspec(float ratio);
        void BuildPerspec(void * w);
        void BuildOrth();

        void SetBuildMethod(unsigned int method);

        void Build(float ratio);
        void Build(void*w);
    };
}

#endif // CAMERA_H_INCLUDED
