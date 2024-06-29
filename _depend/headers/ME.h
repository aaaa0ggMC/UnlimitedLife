#ifndef ME_H_INCLUDED
#define ME_H_INCLUDED

#include <com/macros.h>
#include <com/utility.h>
#include <com/me_math.h>

#include <com/window.h>
#include <com/shader.h>
#include <com/texture.h>
#include <com/font.h>
#include <com/vbo.h>

#include <com/gobject.h>
#include <com/camera.h>
#include <com/model.h>

#include <com/lights.h>
#include <com/program.h>

#include <com/audio.h>

#define MEQuickCreateWindow(WIN,width,height,title,paint,input,camera) \
(WIN).Create(width,height,title,0);\
(WIN).MakeCurrent();\
(WIN).SetPaintFunction(paint);\
(WIN).OnKeyPressEvent(input);\
(WIN).UseCamera(camera)

#define MEApplyCameraPerspecAndReSize(WIN,CAM,fovDeg,near,far)\
(CAM).SetPerspec(deg2rad(fovDeg),near,far);\
(CAM).BuildPerspec(&window);\
(WIN).OnResize([](Window&win,int nw,int nh){\
    win.SetViewport(0,0,nw,nh);\
    if(win.curCam)win.curCam->Build(&window);\
});\

#define MEApplyCameraOrthcAndReSize(WIN,CAM,left,right,bottom,top,znear,zfar)\
(CAM).SetOrth(left,right,bottom,top,znear,zfar);\
(CAM).BuildPerspec(&window);\
(WIN).OnResize([](Window&win,int nw,int nh){\
    win.SetViewport(0,0,nw,nh);\
    if(win.curCam)win.curCam->Build(&window);\
});\

#endif // ME_H_INCLUDED
