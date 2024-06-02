#include <com/shader.h>
#include <com/utility.h>
#include <fstream>

using namespace std;
using namespace me;

Shader::Shader(bool x){
    memset(enabled,0,sizeof(bool) * ME_SHADER_TYPEC);
    vertex = fragment = geometry = 0;
    if(x)program = glCreateProgram();
    else program = 0;
}

void Shader::CreateProgram(){
    if(!program)program = glCreateProgram();
}

int Shader::LoadFromFile(const string&file,GLenum type){
    return LoadFromFile(file.c_str(),type,file.length());
}

int Shader::LoadFromFile(const char * file,GLenum type,size_t sz){
    if(!file){
        ME_SIV("Can't load a file whose path is " "[NULL]!",0);
        return ME_ENO_DATA;
    }
    ///Declarations
    size_t rsz = (sz == 0?strlen(file):sz);
    char * buf;
    int ret;

    if(rsz == 0){
        ME_SIV("Can't load a file whose path is " "[\\0]",1);
        return ME_EEMPTY_STRING;
    }
    rsz = Util::file_size(file);
    if(!rsz){
        ME_SIV("Can't get the file size!",2);
        return ME_EBAD_IO;
    }
    ///Create File
    FILE * f = fopen(file,"r");
    buf = new char[rsz];
    if(!buf)return ME_EBAD_MEM;
    memset(buf,0,sizeof(char) * rsz);
    fread(buf,sizeof(char),rsz,f);
    fclose(f);
    ///Call LoadFromMemory to make shader
    ret = LoadFromMemory(buf,type,rsz);
    ///Free buffer
    delete [] buf;
    ///Return the value
    return ret;
}

int Shader::LoadFromMemory(const string&str,GLenum type){
    return LoadFromMemory(str.c_str(),type,str.length());
}

int Shader::LoadFromMemory(const char * str,GLenum type,size_t sz){
    if(!str){
        ME_SIV("string given NULL",0);
        return ME_ENO_DATA;
    }

    size_t rsz = (sz == 0?strlen(str):sz);
    GLuint *target;

    if(rsz == 0){
        ME_SIV("string given empty",1);
        return ME_EEMPTY_STRING;
    }
    if(type == ME_SHADER_VERTEX){
        if(enabled[0]){
            ME_SIV("vertex" " shader already exists!",2);
            return ME_EALREADY_EXI;
        }
        target = &vertex;
        enabled[0] = true;
    }else if(type == ME_SHADER_FRAGMENT){
        if(enabled[1]){
            ME_SIV("fragment" " shader already exists!",3);
            return ME_EALREADY_EXI;
        }
        target = &fragment;
        enabled[1] = true;
    }else if(type == ME_SHADER_GEOMETRY){
        if(enabled[2]){
            ME_SIV("geometry" " shader already exists!",4);
            return ME_EALREADY_EXI;
        }
        target = &geometry;
        enabled[2] = true;
    }else{
        ME_SIV("Bad type of shader.Only VS,FS & GS are supported now!",5);
        return ME_EBAD_TYPE;
    }
    *target = glCreateShader(type);
    glShaderSource(*target,1,&str,NULL);
    glCompileShader(*target);
    glAttachShader(program,*target);
    ///Free free shader data
    glDeleteShader(*target);
    return ME_ENO_ERROR;
}

void Shader::LinkShader(){
    glLinkProgram(program);
}

void Shader::bind(Shader*s){
    if(!s)glUseProgram(0);
    else glUseProgram(s->program);
}

void Shader::unbind(){
    glUseProgram(0);
}

void Shader::bind(){
    glUseProgram(program);
}

void Shader::Log(){
    Util::PrintProgramLog(program);
    if(enabled[0])Util::PrintShaderLog(vertex);
    if(enabled[1])Util::PrintShaderLog(fragment);
    if(enabled[2])Util::PrintShaderLog(geometry);
}


string Shader::GetLog(){
    string x = "";
    StoreLog(x);
    return x;
}

void Shader::StoreLog(string & v){
    Util::GetProgramLog(program,v);
    if(enabled[0])Util::GetShaderLog(vertex,v);
    if(enabled[1])Util::GetShaderLog(fragment,v);
    if(enabled[2])Util::GetShaderLog(geometry,v);
}

GLuint Shader::GetProgram(){return program;}

int Shader::GetFragmentShader(){
    if(enabled[1])return (int)fragment;
    ME_SIV("No fragment shader!",0);
    return ME_ENO_DATA;
}

int Shader::GetGeometryShader(){
    if(enabled[2])return (int)geometry;
    ME_SIV("No geometry shader!",1);
    return ME_ENO_DATA;
}

int Shader::GetVertexShader(){
    if(enabled[0])return (int)vertex;
    ME_SIV("No vertex shader!",0);
    return ME_ENO_DATA;
}

int Shader::LoadsFromFile(const char * vert,const char * frag,const char * geometry){
    return LoadFromFile(vert,ME_SHADER_VERTEX) |
    LoadFromFile(frag,ME_SHADER_FRAGMENT) |
    (geometry?LoadFromFile(geometry,ME_SHADER_GEOMETRY):ME_ENO_ERROR);
}

int Shader::LoadsFromMem(const char * vert,const char * frag,const char * geometry){
    return LoadFromMemory(vert,ME_SHADER_VERTEX) |
    LoadFromMemory(frag,ME_SHADER_FRAGMENT) |
    (geometry?LoadFromMemory(geometry,ME_SHADER_GEOMETRY):ME_ENO_ERROR);
}


int Shader::LoadLinkLogF(const char * vert,const char * frag,const char * geo){
    int ret = LoadsFromFile(vert,frag,geo);
    LinkShader();
    Log();
    return ret;
}

int Shader::LoadLinkLogM(const char * vert,const char * frag,const char * geo){
    int ret = LoadsFromMem(vert,frag,geo);
    LinkShader();
    Log();
    return ret;
}

ShaderArg Shader::GetUniform(const char * s){
    return ShaderArg(program,glGetUniformLocation(program,s));
}

ShaderArg Shader::GetUniform(const string & s){
    return GetUniform(s.c_str());
}


ShaderArg Shader::operator[](const char * s){
    return ShaderArg(program,glGetUniformLocation(program,s));
}

ShaderArg Shader::operator[](string & s){
    return GetUniform(s.c_str());
}

///ShaderArg
GLfloat ShaderArg::operator=(GLfloat v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform1f(program,offset,v);
    return v;
}

glm::mat3& ShaderArg::operator=(glm::mat3& v){
    return UploadMat3(v);
}
glm::vec4& ShaderArg::operator=(glm::vec4& v){
    return UploadVec4(v);
}
glm::vec3& ShaderArg::operator=(glm::vec3& v){
    return UploadVec3(v);
}
glm::vec2& ShaderArg::operator=(glm::vec2& v){
    return UploadVec2(v);
}

void ShaderArg::UploadRaw(glm::mat4 m){
    this->operator=(glm::value_ptr(m));
}
void ShaderArg::UploadRaw(glm::mat3 m){
    this->UploadMat3(glm::value_ptr(m));
}
void ShaderArg::UploadRaw(glm::vec4 v){
    this->UploadVec4(glm::value_ptr(v));
}
void ShaderArg::UploadRaw(glm::vec3 v){
    this->UploadVec3(glm::value_ptr(v));
}
void ShaderArg::UploadRaw(glm::vec2 v){
    this->UploadVec2(glm::value_ptr(v));
}

GLdouble ShaderArg::UploadDouble(GLdouble v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform1d(program,offset,v);
    return v;
}

GLint ShaderArg::UploadInt(GLint v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform1i(program,offset,v);
    return v;
}

GLuint ShaderArg::operator=(GLuint v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform1ui(program,offset,v);
    return v;
}

GLfloat* ShaderArg::operator=(GLfloat* v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniformMatrix4fv(program,offset,1,GL_FALSE,v);
    return v;
}

GLfloat* ShaderArg::UploadMat3(GLfloat* v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniformMatrix3fv(program,offset,1,GL_FALSE,v);
    return v;
}

GLfloat* ShaderArg::UploadVec3(GLfloat * v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform3f(program,offset,v[0],v[1],v[2]);
    return v;
}

GLfloat* ShaderArg::UploadVec4(GLfloat * v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform4f(program,offset,v[0],v[1],v[2],v[3]);
    return v;
}

GLfloat* ShaderArg::UploadVec2(GLfloat * v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform2f(program,offset,v[0],v[1]);
    return v;
}

glm::vec4& ShaderArg::UploadVec4(glm::vec4& v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform4f(program,offset,v.x,v.y,v.z,v.w);
    return v;
}

glm::vec3& ShaderArg::UploadVec3(glm::vec3& v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform3f(program,offset,v.x,v.y,v.z);
    return v;
}

glm::vec2& ShaderArg::UploadVec2(glm::vec2& v){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return v;
    }
    glProgramUniform2f(program,offset,v.x,v.y);
    return v;
}

void ShaderArg::UploadVec4(float x,float y,float z,float w){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return;
    }
    glProgramUniform4f(program,offset,x,y,z,w);
}

void ShaderArg::UploadVec3(float x,float y,float z){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return;
    }
    glProgramUniform3f(program,offset,x,y,z);
}

void ShaderArg::UploadVec2(float x,float y){
    if(!ava){
        ME_SIV("Uniform not available!",0);
        return;
    }
    glProgramUniform2f(program,offset,x,y);
}

ShaderArg::ShaderArg(GLuint a,GLuint b){
    SetProgram(a);
    SetOffset(b);
}

GLuint ShaderArg::GetOffset(){return offset;}
GLuint ShaderArg::GetProgram(){return program;}
void ShaderArg::SetOffset(GLuint a){
    offset = a;
}

void ShaderArg::SetProgram(GLuint b){
    if(b){
        ava = true;
        program = b;
    }
}

bool ShaderArg::IsAvailable(){return ava;}
glm::mat4& ShaderArg::operator=(glm::mat4 &v){
    (*this) = glm::value_ptr(v);
    return v;
}
glm::mat3& ShaderArg::UploadMat3(glm::mat3 &v){
    UploadMat3((GLfloat*)glm::value_ptr(v));
    return v;
}
