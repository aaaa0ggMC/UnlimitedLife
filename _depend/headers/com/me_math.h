#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED
#include "gobject.h"

namespace me{
    class Velocity{
    public:
        float v;
        glm::vec3 vv;
        void New();
        void Add(int,int,int);
        void Form();

        void SetVelocity(float v);
        void Move(GObject & obj,float etime);
        void MoveDr(GObject & obj,float etime);

        Velocity(float v);
    };
}

#endif // MATH_H_INCLUDED
