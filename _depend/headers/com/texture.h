#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED
#include <GL/glew.h>
#include <com/macros.h>
#include <com/utility.h>

namespace me{
    class Texture : public noncopyable{
    public:
        friend class GlFont;
        Texture();
        //free data
        ~Texture();
        int LoadFromFile(const char * f);
        ///if copy == true,sz isn't effective
        int LoadFromMem(unsigned char * d,size_t sz,bool copy = true);
        int Create2DTextureArray(unsigned int width,unsigned int height,unsigned int depth,GLuint fmt = GL_RGBA,GLuint sourcefmt = GL_RED,unsigned char * source = 0);
        int CreateTextureBuffer(unsigned int size,unsigned char * init = NULL);
        int UpdateGLTexture1D(unsigned char * bytes,unsigned int size,unsigned int offset);
        int UpdateGLTexture(unsigned char * bytes,unsigned int depth_offset,unsigned int w,unsigned int h,unsigned int depth = 1,GLuint format = GL_RGBA,unsigned int alignValue = 4,bool clearTex = false);
        int ClearGLTexture(unsigned int start_off_contained = 0,unsigned int end_off_contained = 0,float r = 0,float g = 0,float b = 0,float a = 0);
        int UploadToOpenGL(bool genMipMap = true,int rtp = GL_REPEAT,unsigned int minft = GL_LINEAR_MIPMAP_NEAREST,unsigned int maxft = GL_LINEAR_MIPMAP_NEAREST);
        GLuint GetHandle();
        void Activate(GLuint index);
        static void Deactivate(GLuint);
        void FreeData();
        void FreeTexture();

        static void InitFramebuffer();

        unsigned char * data;
        GLuint handle;
        GLenum type;

        unsigned int width,height;
        unsigned int depth;
        GLuint format;
        unsigned int channels;

        bool deleteS;
        bool dataAva;

        ///Used to clear texture data
        static GLuint frameBuffer;
        static bool inited;
    };
}

#endif // TEXTURE_H_INCLUDED
