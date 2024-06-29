#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED
#include <com/utility.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <thread>
#include <unordered_map>
#include <com/camera.h>
#include <com/model.h>
#include <com/gobject.h>
#include <com/utility.h>

namespace me{
    using namespace std;
    class Window : public noncopyable{
    public:
        typedef void (*WPaintFn)(Window&,double currentTime,Camera*cam);
        typedef void (*OnKeyPress)(Window&,double elapseus,Camera*cam);
        typedef void (*DiscreteKeyListener)(GLFWwindow * win,int key,int scancode,int action,int mods);
        typedef void (*OnResizeFn)(Window& w,int new_w,int new_h);

        static Window * GetCurrent();
        static void MakeCurrent(Window *);
        //默认不开启垂直同步
        static void SetSwapInterval(unsigned int = 0);

        Window(int major = 4,int minor = 3);

        int Create(unsigned int width,unsigned int height,const char * title,Window* share=NULL);
        int Create(unsigned int width,unsigned int height,std::string title,Window* share=NULL);

        void Destroy();
        bool ShouldClose();
        void Display();
        void Clear(bool clearColor = true,bool clearDepth = true);
        static void SetViewport(int x,int y,unsigned int sizex,unsigned int sizey);

        void PollEvents();

        GLFWwindow * GetGLFW();
        long GetSystemHandle();

        void MakeCurrent();
        unsigned int GetFramerateLimit();
        void SetFramerateLimit(unsigned int limit);

        void SetCheckInputInterval(unsigned int interval = 120);

        void Draw(GObject&,GLuint targetC,GLuint instance = 1);
        void DrawModel(Model & model,GLuint instance = 1,GLuint bindingIndex = 0,GLuint=2);

        void SetPaintFunction(WPaintFn);
        void OnKeyPressEvent(OnKeyPress);
        void OnResize(OnResizeFn fn);
        void SetDiscreteKeyListener(DiscreteKeyListener l);
        void UseCamera(Camera& cam);

        void EnableDepthTest(GLenum = GL_LEQUAL);
        void DisableDepthTest();

        void EnableCullFaces();
        void DisableCullFaces();

        void SetFrontFace(unsigned int dr);

        bool KeyInputed(int key,int state = GLFW_PRESS);

        void SetUIRange(float left,float top,float right,float bottom);

        void __Display_Pre();
        void __Display_Aft();

        glm::vec2 GetWindowSize();
        glm::vec2 GetBufferSize();

        Camera uiCam;
        Camera * curCam;
    private:
        static Window * current;
        static std::unordered_map<GLFWwindow*,Window*> instances;
        std::thread intervalThread;
        GLFWwindow* win;
        WPaintFn paint;
        OnKeyPress press;
        OnResizeFn onResizeFunc;
        DiscreteKeyListener keyListener;
        unsigned int flimit;
        float twait,frame_start;
        unsigned int interval;
        bool limitedF;
        float firstTime;
        bool isOpen;
    };
}

#endif // WINDOW_H_INCLUDED
