#ifndef PROGRAM_H_INCLUDED
#define PROGRAM_H_INCLUDED
#include "gobject.h"
#include <vector>

namespace me{
    using namespace std;
    class Program{
    public:
        vector<GObject*> objs;
        void PushObj(vector<GObject*> l);
        void Update();
    private:

    };
}

#endif // PROGRAM_H_INCLUDED
