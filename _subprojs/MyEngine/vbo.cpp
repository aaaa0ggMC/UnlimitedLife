#include <com/vbo.h>

using namespace me;
using namespace std;

VBO::VBO(GLuint v,unsigned int vbot,unsigned int stride){
    vbo = v;
    drawMethod = GL_TRIANGLES;
    vbo_type = vbot;
    tps = 3;
    this->stride = stride;
}

void VBO::Set(vector<GLfloat> v){(*this) = v;}
void VBO::Set(GLfloat d[],size_t sz){
    if(!vbo || !sz)return;
    glBindBuffer(vbo_type,vbo);
    glBufferData(vbo_type,sz,d,GL_STATIC_DRAW);
}
vector<GLfloat>* VBO::operator=(vector<GLfloat> & v){
    if(!vbo)return &v;
    size_t sz = v.size() * sizeof(GLfloat);
    glBindBuffer(vbo_type,vbo);
    glBufferData(vbo_type,sz,&v[0],GL_STATIC_DRAW);
    return &v;
}

void VBO::Set(vector<GLint> v){(*this) = v;}
void VBO::Set(GLint d[],size_t sz){
    if(!vbo || !sz)return;
    glBindBuffer(vbo_type,vbo);
    glBufferData(vbo_type,sz,d,GL_STATIC_DRAW);
}
vector<GLint>* VBO::operator=(vector<GLint> & v){
    if(!vbo)return &v;
    size_t sz = v.size() * sizeof(GLint);
    glBindBuffer(vbo_type,vbo);
    glBufferData(vbo_type,sz,&v[0],GL_STATIC_DRAW);
    return &v;
}

void VBO::SetVBO(GLuint v){vbo = v;}
GLuint VBO::GetVBO(){return vbo;}



void VBO::bind(){
    glBindBuffer(vbo_type,vbo);
}


void VBO::AttributePointer(GLuint index,GLuint typesPerI,GLenum type,GLboolean normalized,GLsizei stride,const void * pointer){
    glVertexAttribPointer(index,typesPerI,type,normalized,stride,pointer);
}

void VBO::EnableArray(GLuint index){
    glEnableVertexAttribArray(index);
}

void VBO::unbind(){
    this->bind();
    glDisableVertexAttribArray(bidx);
}


void VBO::bind2(GLuint index){
    this->bind();
    if(vbo_type == ME_VBO_ELEMENT){
        ME_SIV("Element Buffer doesn't support being visited in shaders!",0);
        return;
    }
    AttributePointer(index,tps,GL_FLOAT,GL_FALSE,stride,0);
    bidx = index;
    EnableArray(index);
}

GLuint VBO::CreateNew(){
    ME_SIV("OpenGL requires to gen vertex arrays at a fixed point!",2);
    if(vbo){
        ME_SIV("already created vbo!",0);
        return vbo;
    }
    glGenVertexArrays(1,&vbo);
    if(vbo){
        return vbo;
    }
    ME_SIV("gen vbo failed",1);
    return 0;
}

///VBOs
VBOs::VBOs(){vbos.push_back(VBO(0));}
void VBOs::AppendVBOs(unsigned int c,unsigned int tp){
    GLuint *gvbo = new GLuint[c];
    glGenBuffers(c,gvbo);
    for(unsigned int i = 0; i < c;++i){
        //cout << gvbo[i] << endl;
        vbos.push_back(VBO(gvbo[i],tp));
    }
}
vector<GLuint> VBOs::GetGLVBOs(){
    vector<GLuint> uv;
    for(const VBO & va : vbos){
        uv.push_back(va.vbo);
    }
    uv.erase(uv.begin(),uv.begin()+1);
    return uv;
}

vector<VBO> VBOs::GetVBOs(){
    vector<VBO> ret = vbos;
    ret.erase(ret.begin(),ret.begin()+1);
    return ret;
}

VBO& VBOs::operator[](unsigned int index){
    if(index+1 >= vbos.size())return vbos[0];
    return vbos[index+1];
}
