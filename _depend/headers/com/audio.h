#ifndef MUSICCONTROLLER_H_INCLUDED
#define MUSICCONTROLLER_H_INCLUDED

#include <string>
#include <vector>
#include <com/utility.h>

using namespace std;
#define DO_PRAGMA(x) _Pragma (#x)

#ifdef USE_SFML
    #include <SFML/Audio.hpp>
    using namespace sf;
    using pMusic = Music*;
    #pragma GCC diagnostic pop
#else
    using pMusic = void*;
#endif // USE_SFML

struct MusicStruct{
    string givenId;
    string fileName;
    pMusic music;
    bool loop;
    bool keep;
};

struct MusicController{
    vector<MusicStruct> holded;
    //int SuspendPlay(string filePath,bool keep = true,string given = "",bool loop = false);
    int AppendPlay(string filePath,bool keep = true,string given = "",bool loop = false);
    void Update();
    void PauseAll();
    void ResumeAll();
    ~MusicController();
    pMusic operator[](string id);
};

#endif // MUSICCONTROLLER_H_INCLUDED
