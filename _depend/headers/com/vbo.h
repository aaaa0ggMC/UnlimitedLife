#ifndef VBO_H_INCLUDED
#define VBO_H_INCLUDED
#include <GL/glew.h>
#include <vector>
#include <com/macros.h>
#include <com/utility.h>

namespace me{
    class VBO{
    public:
        unsigned int vbo_type;
        GLuint drawMethod;
        //vertices per shape
        unsigned int tps;
        unsigned int stride;

        VBO(GLuint v = 0,unsigned int vbo_type = ME_VBO_VERTEX,unsigned int=0);

        vector<GLfloat>* operator=(vector<GLfloat>& v);
        vector<GLint>* operator=(vector<GLint>& v);
        void Set(vector<GLfloat> v);
        void Set(GLfloat *d,size_t sz);
        void Set(vector<GLint> v);
        void Set(GLint *d,size_t sz);

        void bind();
        void bind2(GLuint index);

        void unbind();

        GLuint GetVBO();
        GLuint CreateNew();

        static void AttributePointer(GLuint bindingIndex,GLuint typesPerI = 3,GLenum type = GL_FLOAT,GLboolean normalized = GL_FALSE,GLsizei stride = 0,const void * pointer = NULL);
        static void EnableArray(GLuint index);
    private:
        friend class VBOs;
        void SetVBO(GLuint v);
        GLuint vbo;
        unsigned int bidx;
    };

    class VBOs{
    public:
        VBO& operator[](unsigned int index);
        vector<VBO> GetVBOs();
        vector<GLuint> GetGLVBOs();
        void AppendVBOs(unsigned int count,unsigned int tp = ME_VBO_VERTEX);
        VBOs();
    private:
        vector<VBO> vbos;
    };
}

#endif // VBO_H_INCLUDED
