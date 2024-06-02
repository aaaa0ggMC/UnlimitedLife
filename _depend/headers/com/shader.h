#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <com/macros.h>
#include <com/utility.h>

namespace me{
    using namespace std;
    class ShaderArg {
    public:
        ShaderArg(GLuint program = 0,GLuint offset = 0);
        GLuint GetOffset();
        GLuint GetProgram();
        void SetOffset(GLuint);
        void SetProgram(GLuint);
        bool IsAvailable();

        GLfloat operator=(GLfloat v);
        GLfloat* operator=(GLfloat* v);
        GLuint operator=(GLuint v);
        glm::mat4& operator=(glm::mat4& v);
        glm::mat3& operator=(glm::mat3& v);
        glm::vec4& operator=(glm::vec4& v);
        glm::vec3& operator=(glm::vec3& v);
        glm::vec2& operator=(glm::vec2& v);

        GLdouble UploadDouble(GLdouble v);
        GLint UploadInt(GLint v);
        GLfloat* UploadVec4(GLfloat *);
        GLfloat* UploadVec3(GLfloat *);
        GLfloat* UploadVec2(GLfloat *);
        glm::vec4& UploadVec4(glm::vec4&);
        glm::vec3& UploadVec3(glm::vec3&);
        glm::vec2& UploadVec2(glm::vec2&);
        void UploadVec4(float x,float y,float z,float w);
        void UploadVec3(float x,float y,float z);
        void UploadVec2(float x,float y);
        GLfloat* UploadMat3(GLfloat *);
        glm::mat3& UploadMat3(glm::mat3&);
        void UploadRaw(glm::mat4);
        void UploadRaw(glm::mat3);
        void UploadRaw(glm::vec4);
        void UploadRaw(glm::vec3);
        void UploadRaw(glm::vec2);
    private:
        bool ava;
        GLuint program;
        GLuint offset;
    };

    class Shader{
    public:
        Shader(bool initProgram = false);
        //create the shader program if program is not inited at constrcution
        void CreateProgram();
        //get uniform
        ShaderArg GetUniform(const char * s);
        ShaderArg GetUniform(const string & s);
        ShaderArg operator[](const char * s);
        ShaderArg operator[](string & s);
        ///load shaders
        //load&link&log
        int LoadLinkLogF(const char * vert,const char * frag,const char * geo = NULL);
        int LoadLinkLogM(const char * vert,const char * frag,const char * geo = NULL);
        //only load
        int LoadFromFile(const string&file,GLenum type);
        int LoadFromFile(const char * file,GLenum type,size_t sz = ME_DETECT_SIZE);
        int LoadFromMemory(const string&str,GLenum type);
        int LoadFromMemory(const char * str,GLenum type,size_t sz = ME_DETECT_SIZE);
        //load many
        int LoadsFromFile(const char * vert,const char * frag,const char * geometry = NULL);
        int LoadsFromMem(const char * vert,const char * frag,const char * geometry = NULL);
        //get ids,returns number below 0 if target isn't available
        int GetVertexShader();
        int GetFragmentShader();
        int GetGeometryShader();
        //returns 0 if program hasn't been created
        GLuint GetProgram();

        //bind shader
        void bind();
        //bind shader,pass NULL to unbind
        static void bind(Shader*);
        //unbind
        static void unbind();

        //link shader
        void LinkShader();
        //print logs
        void Log();
        //get logs
        string GetLog();
        //append logs
        void StoreLog(string& appender);
    private:
        friend class Window;
        GLuint program;
        GLuint vertex,fragment,geometry;
        bool enabled[ME_SHADER_TYPEC];
    };
}

#endif // SHADER_H_INCLUDED
