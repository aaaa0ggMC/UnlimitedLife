#ifndef GLSUPPORT_H_INCLUDED
#define GLSUPPORT_H_INCLUDED
#include <GL/glew.h>

namespace me{
    struct GLSupport{
        GLSupport() = delete;
        void operator=(const GLSupport&) = delete;

        enum GLType{
            AnisotropicFilter
        };

        static bool Check(GLType tp);
        static bool Enable(GLType tp,float v);
    };
}

#endif // GLSUPPORT_H_INCLUDED
