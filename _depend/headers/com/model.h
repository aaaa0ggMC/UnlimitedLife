#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED
#include <com/gobject.h>
#include <vector>
#include <com/vbo.h>
#include <com/utility.h>

namespace me{
    using namespace std;
    struct Model : public GObject{
//        VBO ivbo;
        VBO nvbo;
        vector<float> vfloats;
//        vector<int> indices;
        vector<float> nfloats;
        vector<float> tfloats;
        unsigned int vertc;
        bool hasNormal;

        int LoadModelFromObj(const char * fname);
        int LoadModelFromStlBin(const char * fname);
        void UploadToOpenGL();
        void CreateVBOs(VBO& v,VBO& n);
        void SetBindings(GLuint vertex,GLuint);
        void Unbind();
        void SetData(vector<float>*vert,vector<float>*texc=NULL,vector<float>*norm=NULL);

        Model(float x,float y,float z = 0);
    };

    struct Model1 : public GObject{
        Model1(float x=0,float y=0,float z=0);

        void BindVBos(VBO vert,VBO indi,VBO texc = VBO(),VBO norm = VBO());
    };


    class ObjLoader{
    public:
        ObjLoader(vector<float>&vf,vector<float>& nm,vector<float>& tc,unsigned int& fc);
        bool LoadFromObj(const char * obj);
        bool LoadFromStlBin(const char * fp);

        vector<glm::vec3> vertices;
        vector<glm::vec3> normals;
        vector<glm::vec2> tcoords;

        vector<float>& vfloats;
        vector<float>& vnormals;
        vector<float>& vtexc;

//        vector<int> vindices;
//        vector<int> nindices;
//        vector<int> tindices;

        unsigned int& vertc;
    };
}

#endif // MODEL_H_INCLUDED
