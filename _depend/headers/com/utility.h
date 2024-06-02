#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED
#include <string>
#include <unordered_set>
#include <com/macros.h>

#include "util/GPUInfo.h"
#include "util/glsupport.h"

namespace me{
    using namespace std;
    class Util{
    public:
        ///created for InvokeConsole
        static unordered_set<string> sessions;
        static bool initedGlew;
        static bool initedGLFW;

        //Get OpenGL errors,appender is a string used to append the errors,sig is the prefix of the line
        //Returns whether there exists opengl errors
        static bool GetOpenGLError(string&appender,const char * sigStr = "Find OpenGL Error:");
        //print opengl errors if exists
        static void PrintOpenGLError();
        //get the size of a file,given the file path
        static size_t file_size(const char*filename);
        //init gl extension wrangler
        static int InitGlew();
        //init gl framework
        static void InitGLFW(int major = 4,int minor = 3);
        //given the shader id,append the log after appender,returns...
        static bool GetShaderLog(GLuint shader,string&appender);
        //print the shader logs if extists
        static void PrintShaderLog(GLuint shader);
        //get the whole program's log
        static bool GetProgramLog(GLuint prog,string&appender);
        //print the program's log if exists
        static void PrintProgramLog(GLuint prog);
        //register terminal function,used to free glfw resources
        static void RegisterTerminal();
        static void OnTerminal();
        //invoke an error to the console,requires define DEBUG macros!
        static void InvokeConsole(const char * s,bool onlyOnce = false,const char * sessionId = NULL,long = 0);

        ///prevent instancing
        Util() = delete;
        ~Util() = delete;
        void operator=(Util&) = delete;
    };

    class noncopyable
    {
     public:
      noncopyable(const noncopyable&) = delete;
      void operator=(const noncopyable&) = delete;
     protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };

    class Counter{
    public:
        Counter(bool start = false);
        float GetCyclePerS();
        void ReStart();
        void Stop();
        void SimpOut();
        void Increase();
        double start,end;
        unsigned long cycles;
    };

    class Changer{
    public:
        int dirty;
        void MarkDirty(int mask=1);
        int GetDirty();
        bool Match(int mask = 1);
        Changer();
        virtual void Update(unsigned int=0) = 0;
    };
}


#endif // UTILITY_H_INCLUDED
