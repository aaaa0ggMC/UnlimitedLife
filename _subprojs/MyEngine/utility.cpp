#include <GL/glew.h>
#include <iostream>
#include <sys/stat.h>
#include <GLFW/glfw3.h>
//卑鄙的手段临时访问LogFactory
#define private public
#include <com/utility.h>
#undef private

#define _SimpOut(X) \
if(!(!logger)){\
    alib::ng::LogFactory & lgt = **logger;\
    lgt(LOG_ERROR,APCF_LIGHT_YELLOW) << X << alib::ng::endlog;\
}else{\
    std::cout << X << alib::ng::endlog;\
}\

using namespace me;
using namespace std;
namespace me{
    struct AUtilImpl{
        bool inited;
        bool initedGlew;
        bool initedGLFW;

        AUtilImpl(){
            inited = initedGlew = initedGLFW = false;
        }
    };
}
unordered_set<string> Util::sessions;
AUtilImpl* Util::impl = new AUtilImpl();
bool Util::out2console = true;
optional<alib::ng::LogFactory*> Util::logger = std::nullopt;

bool Util::GetOpenGLError(std::string&appender,const char * sigStr){
    bool hasError = false;
    GLenum glErr;
    glErr = glGetError();
    while(glErr != GL_NO_ERROR){
        appender.append(sigStr);
        appender.append(to_string(glErr));
        appender.append("\n");
        hasError = true;
        glErr = glGetError();
    }
    return hasError;
}

void Util::PrintOpenGLError(){
    string inner = "";
    if(GetOpenGLError(inner)){
       _SimpOut("[OpenGL Error]" << inner);
    }
}

size_t Util::file_size(const char*filename){
    struct stat statbuf;
    int ret;
    ret = stat(filename,&statbuf);//调用stat函数
    if(ret != 0) return 0;//获取失败。 2023 6 5:这里有改动
    return statbuf.st_size;//返回文件大小。
}

int Util::InitGlew(){
    if(!(impl->initedGlew)){
        int ret = glewInit();
        if(!ret)impl->initedGlew = true;
        return ret;
    }
    return GLEW_OK;
}

void Util::InitGLFW(int major,int minor){
    if(!impl->initedGLFW){
        impl->initedGLFW = true;
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,minor);
    }
}

bool Util::GetShaderLog(GLuint shader,string&appender){
    int len = 0,chWritten = 0;
    glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&len);
    if(len <= 0)return false;
    char * data = new char[len];
    glGetShaderInfoLog(shader,len,&chWritten,data);
    appender.append(data);
    delete [] data;
    return true;
}

void Util::PrintShaderLog(GLuint shader){
    string s = "";
    bool ret = GetShaderLog(shader,s);
    if(ret){
        _SimpOut(s);
    }
}


bool Util::GetProgramLog(GLuint shader,string&appender){
    int len = 0,chWritten = 0;
    glGetProgramiv(shader,GL_INFO_LOG_LENGTH,&len);
    if(len <= 0)return false;
    char * data = new char[len];
    glGetProgramInfoLog(shader,len,&chWritten,data);
    appender.append(data);
    delete [] data;
    return true;
}

void Util::PrintProgramLog(GLuint shader){
    string s = "";
    bool ret = GetProgramLog(shader,s);
    if(ret){
        _SimpOut(s);
    }
}

void Util::Init(){
    if(impl->inited)return;
    std::atexit(Util::OnTerminal);
    impl->inited = true;
}

void Util::OnTerminal(){
    glfwTerminate();
    delete *logger;
    delete impl;
}

void Util::InvokeConsole(const char * s,bool onlyOnce,const char * sessionId,long sig){
    string sd = sessionId?sessionId:"";
    sd += " : ";
    sd += to_string(sig);
    auto it = sessions.find(sd);
    if(onlyOnce &&  it != sessions.end())return;
    if(it == sessions.end() && sessionId)sessions.insert(sd);
    _SimpOut("[" << sd << "]" << s);
}

///Counter
Counter::Counter(bool st){
    cycles = 0;
    end = 0;
    if(st)ReStart();
}

float Counter::GetCyclePerS(){
    double elapse = (end - start);
    return cycles / elapse;
}

void Counter::ReStart(){
    start = glfwGetTime();
}

void Counter::Stop(){
    end = glfwGetTime();
}

void Counter::Increase(){++cycles;}

void Counter::SimpOut(){
    cout << "FPS average:" << (end-start)*1000 << "ms " << GetCyclePerS() << "cycles/s\n";
}

///Changer
Changer::Changer(){
    dirty = 0;
}

void Changer::MarkDirty(int m){
    dirty |= m;
}

int Changer::GetDirty(){
    return dirty;
}

bool Changer::Match(int m){
    return dirty & m;
}

///GLSupport
bool GLSupport::Check(GLSupport::GLType tp){
    switch(tp){
    case AnisotropicFilter:
        return glewIsSupported(EXT_AF);
    }
    return false;
}

bool GLSupport::Enable(GLSupport::GLType tp,float v){
    if(!Check(tp)){
        Util::InvokeConsole("given option isn't supported!",false);
        return false;
    }
    switch(tp){
        case AnisotropicFilter:{
            GLfloat sett;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&sett);
            if( v>=0 && v<sett)sett = v;
            glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,sett);
            return true;
        }
    }
    return false;
}

void Util::SetLogger(alib::ng::Logger & lg){
    logger = new alib::ng::LogFactory("MyEngine",lg);
}

void Util::ToggleConsoleOutput(bool v){
    out2console = v;
}
