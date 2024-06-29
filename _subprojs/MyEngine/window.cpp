#define GLFW_EXPOSE_NATIVE_WIN32
#include <com/window.h>
#include <GLFW/glfw3native.h>
#include <com/texture.h>

using namespace std;
using namespace me;

Window* Window::current = NULL;
std::unordered_map<GLFWwindow*,Window*> Window::instances;

void Window::Clear(bool clearColor,bool clearDepth){
    if(clearColor)glClear(GL_COLOR_BUFFER_BIT);
    if(clearDepth)glClear(GL_DEPTH_BUFFER_BIT);
}

void Window::PollEvents(){
    glfwPollEvents();
}

void Window::EnableDepthTest(GLenum func){
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(func);
}

void Window::DisableDepthTest(){
	glDisable(GL_DEPTH_TEST);
}

void Window::EnableCullFaces(){
    glEnable(GL_CULL_FACE);
}
void Window::DisableCullFaces(){
    glDisable(GL_CULL_FACE);
}

void Window::SetFrontFace(unsigned int dr){
    glFrontFace(dr);
}

Window::Window(int major,int minor){
    Util::Init();
    Util::InitGLFW(major,minor);
    paint = NULL;
    win = NULL;
    curCam = NULL;
    onResizeFunc = NULL;
    flimit = frame_start = twait = 0;
    limitedF = false;
    press = NULL;
    interval = 120;
    isOpen = false;
}

void Window::SetCheckInputInterval(unsigned int i){
    interval = i;
}

void Window::SetUIRange(float l,float t,float r,float b){
    uiCam.BuildOrth(l,r,b,t);
}

int Window::Create(unsigned int width,unsigned int height,const char* title,Window*share){
    if(win){
        ME_SIV("The window is already created!",0);
        return ME_EALREADY_EXI;
    }
    win = glfwCreateWindow(width,height,title,NULL,share?share->win:NULL);
    isOpen = true;
    ///Create Input Interval
    intervalThread = std::thread(
    [](bool*isOpen,OnKeyPress*press,unsigned int*interval,Window*win){
        while(*isOpen){
            if(*press)(*press)(*win,1.0 / (*interval),win->curCam);
            std::this_thread::sleep_for(1000ms / (*interval));
        }
    },&isOpen,&press,&interval,this);
    instances.emplace(win,this);
    return win?ME_ENO_ERROR:ME_EBAD_MEM;
}

void Window::SetDiscreteKeyListener(Window::DiscreteKeyListener l){
    keyListener = l;
    glfwSetKeyCallback(win,l);
}


int Window::Create(unsigned int width,unsigned int height,std::string title,Window*share){
    return Create(width,height,title.c_str(),share);
}

GLFWwindow * Window::GetGLFW(){return win;}

long Window::GetSystemHandle(){
    return (long long)glfwGetWin32Window(win);
}

Window* Window::GetCurrent(){return current;}

void Window::MakeCurrent(){
    glfwMakeContextCurrent(this->win);
    current = this;
    Util::InitGlew();
    ///here we init framebuffer secretly
    Texture::InitFramebuffer();
}

void Window::MakeCurrent(Window * v){
    if(v){
        glfwMakeContextCurrent(v->win);
    }else glfwMakeContextCurrent(NULL);
    current = v;
    Util::InitGlew();
}

void Window::Destroy(){
    isOpen = false;
    glfwDestroyWindow(win);
    if(intervalThread.joinable())intervalThread.join();
    auto tg = instances.find(win);
    this->win = NULL;
    if(tg != instances.end()){
        instances.erase(tg);
    }
    MakeCurrent(NULL);
}

void Window::SetSwapInterval(unsigned int a){
    glfwSwapInterval(a);
}

bool Window::ShouldClose(){
    return glfwWindowShouldClose(win);
}

void Window::__Display_Pre(){
    static bool warned = false;
    if(!curCam && !warned){
        warned = true;
        ME_SIV("The main camera is NULL!Game may crash!Use Window::UseCamera to activate!",0);
    }
}

void Window::__Display_Aft(){
    firstTime = glfwGetTime();
    glfwSwapBuffers(win);
    if(limitedF){
        while(glfwGetTime()+ME_FRAME_ADJUST_V < frame_start + twait){
            std::this_thread::sleep_for((twait)*100ms);
        }
        frame_start += twait;
    }
}

void Window::Display(){
    __Display_Pre();
    if(paint)paint(*this,glfwGetTime(),curCam);
    __Display_Aft();
}


bool Window::KeyInputed(int key,int state){
    return glfwGetKey(win,key) == state;
}

void Window::SetPaintFunction(WPaintFn fn){
    paint = fn;
}

void Window::OnKeyPressEvent(OnKeyPress fn){
    press = fn;
}

void Window::SetFramerateLimit(unsigned int limit){
    if(!limit){
        limitedF = false;
        flimit = UINT_MAX;
        return;
    }
    limitedF = true;
    frame_start = glfwGetTime();
    flimit = limit;
    twait = 1.0 / flimit;
}

unsigned int Window::GetFramerateLimit(){
    return flimit;
}

void Window::UseCamera(Camera & c){
    curCam = &c;
}

glm::vec2 Window::GetWindowSize(){
    int w,h;
    glfwGetWindowSize(win,&w,&h);
    return glm::vec2(w,h);
}

void Window::Draw(GObject& o,GLuint verts,GLuint in){
    if(!in){
        ME_SIV("The count of instance is zero.",0);
        return;
    }
    if(o.vbo.GetVBO()){
        o.vbo.bind2(o.vbind);
        if(o.coord.GetVBO()){
            if(o.cbind != o.vbind)o.coord.bind2(o.cbind);
            else{
                ME_SIV("How can u just bind vertex buffer & coord buffer together?",1);
            }
        }
        if(in <= 1)glDrawArrays(o.vbo.drawMethod, 0, verts);
        else glDrawArraysInstanced(o.vbo.drawMethod,0,verts,in);
    }
}


void Window::DrawModel(Model & model,GLuint in,GLuint vert,GLuint norm){
    if(!in){
        ME_SIV("The count of instance is zero.",0);
        return;
    }
    model.SetBindings(vert,norm);
    if(in <= 1)glDrawArrays(model.vbo.drawMethod,0,model.vertc);
    else glDrawArraysInstanced(model.vbo.drawMethod,0,model.vertc,in);
    model.Unbind();
}

glm::vec2 Window::GetBufferSize(){
    int w,h;
    glfwGetFramebufferSize(win,&w,&h);
    return glm::vec2(w,h);
}

void Window::SetViewport(int x,int y,unsigned int sx,unsigned int sy){
    glViewport(x,y,sx,sy);
}

void Window::OnResize(Window::OnResizeFn fn){
    static bool setted = false;
    onResizeFunc = fn;
    if(!setted){
        setted = true;
        glfwSetWindowSizeCallback(GetGLFW(),[](GLFWwindow* glwin,int nx,int ny){
            auto instance = Window::instances.find(glwin);
            if(instance != instances.end()){
                //防止glm出错
                if(nx == 0)nx = 1;
                if(ny == 0)ny = 1;
                if((instance->second)->onResizeFunc)(instance->second)->onResizeFunc(*(instance->second),nx,ny);
            }else{
                ME_SIVD("Failed to get current instance!",0);
            }
        });
    }
}
