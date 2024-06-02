#include <com/texture.h>
#include <com/utility.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_DDS
#define STBI_NO_PVR
#define STBI_NO_PKM
#define STBI_NO_QOI
#define STBI_NO_EXT
#include <stb_image.h>

using namespace me;
using namespace std;

GLuint Texture::frameBuffer = 0;
bool Texture::inited = false;

Texture::Texture(){
    data = NULL;
    handle = 0;
    type = GL_TEXTURE_2D;
    channels = width = height = depth = 0;
    deleteS  = false;
    dataAva = false;
    format = 0;
}

int Texture::CreateTextureBuffer(unsigned int size,unsigned char * init){
    if(handle){
        ME_SIV("This texture had been gened.Use FreeTexture;btw u can call FreeData along with FreeTexture",0);
        return ME_EALREADY_EXI;
    }
    type = GL_TEXTURE_BUFFER;
    this->format = format;
    width = size;
    glGenTextures(1,&handle);
    glBindTexture(type,handle);
    if(init)glBufferData(type,size,init,GL_STATIC_DRAW);
    return ME_ENO_ERROR;
}

int Texture::UpdateGLTexture1D(unsigned char * bytes,unsigned int size,unsigned int offset){
    if(!handle){
        ME_SIV("Empty texture,use CreateTextureBuffer to generate one",0);
        return ME_EALREADY_EXI;
    }
    if(bytes){
        glBindTexture(type,handle);
        if(type == GL_TEXTURE_BUFFER)glBufferSubData(type,offset,size,bytes);
        else {
            ME_SIV("Unavailable...",1);
        }
    }
    return ME_ENO_ERROR;
}

void Texture::InitFramebuffer(){
    if(!Texture::inited){
        Texture::inited = true;
        glGenFramebuffers(1,&frameBuffer);
        atexit([]{
            glDeleteFramebuffers(1,&Texture::frameBuffer);
        });
    }
}

int Texture::LoadFromFile(const char * f){
    if(data){
        ME_SIV("data already created!",2);
        return ME_EALREADY_EXI;
    }
    if(!f){
        ME_SIV("given [NULL]",0);
        return ME_ENO_DATA;
    }
    size_t sz = strlen(f);
    if(!sz){
        ME_SIV("given \\0",1);
        return ME_EEMPTY_STRING;
    }
    sz = Util::file_size(f);
    if(!sz){
        ME_SIV("empty file!",2);
        return ME_EBAD_IO;
    }
    int w,h,ncs;
    unsigned char * d = stbi_load(f,&w,&h,&ncs,0);
    if(!d){
        string str = "Can't load file ";
        str += f;
        ME_SIV(str.c_str(),3);
        return ME_EBAD_IO;
    }
    data = d;
    channels = ncs;
    width = w;
    height = h;
    deleteS = true;
    dataAva = true;
    return ME_ENO_ERROR;
}

int Texture::LoadFromMem(unsigned char * d,size_t sz,bool copy){
    if(data){
        ME_SIV("data already created!",2);
        return ME_EALREADY_EXI;
    }
    if(!d){
        ME_SIV("given [NULL]",0);
        return ME_ENO_DATA;
    }
    if(copy && !sz){
        ME_SIV("If you want to copy the data,please give the size of the chunk.",1);
        return ME_EBAD_MEM;
    }
    if(copy){
        data = new unsigned char[sz];
        memcpy(data,d,sz);
    }else data = d;
    dataAva = true;
    return ME_ENO_ERROR;
}

int Texture::UploadToOpenGL(bool gmm,int rtp,unsigned int a,unsigned int b){
    GLuint casel = GL_RGB;

    if(!data){
        ME_SIV("You DID NOT load the data!",0);
        return ME_ENO_DATA;
    }
    if(type != GL_TEXTURE_2D){
        if(handle){
            ME_SIV("Incompatible type to load texture!Texture already exists!",0);
            return ME_EALREADY_EXI;
        }else type = GL_TEXTURE_2D;
    }

    if(!handle)glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, rtp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, rtp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, a);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, b);

    switch(channels){
    case 1:
        casel = GL_RED;
        break;
    case 2:
        casel = GL_LUMINANCE_ALPHA;
        break;
    case 4:
        casel = GL_RGBA;
        break;
    case 3:
        break;
    default:
        ME_SIV("Image file is empty",1);
        return ME_ENO_DATA;
    }
    format = casel;
    glTexImage2D(GL_TEXTURE_2D, 0, casel, width, height, 0, casel, GL_UNSIGNED_BYTE, data);
    if(gmm)glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D,0);

    return ME_ENO_ERROR;
}

GLuint Texture::GetHandle(){
    if(handle)return handle;
    ME_SIV("You didn't call UploadToOpenGL to upload the texture!",0);
    return 0;
}

void Texture::Activate(GLuint index){
    if(index >= 32){
        ME_SIV("only supports index below 32",0);
        return;
    }
    glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(type,handle);
	glBindTextureUnit(index,handle);
}

Texture::~Texture(){
    if(dataAva){
        if(deleteS){
            stbi_image_free(data);
        }else delete [] data;
    }
}

void Texture::Deactivate(GLuint i){
    glActiveTexture(GL_TEXTURE0 + i);
}

void Texture::FreeData(){
    if(deleteS){
        stbi_image_free(data);
    }else delete [] data;
    dataAva = false;
    data = NULL;
}

void Texture::FreeTexture(){
    glDeleteTextures(1,&handle);
    handle = 0;
    type = GL_TEXTURE_2D;
    format = 0;
}

int Texture::Create2DTextureArray(unsigned int w,unsigned int h,unsigned int d,GLuint format,GLuint sf,unsigned char* s){
    if(handle){
        ME_SIV("This texture had been gened.Use FreeTexture;btw u can call FreeData along with FreeTexture",0);
        return ME_EALREADY_EXI;
    }
    type = GL_TEXTURE_2D_ARRAY;
    this->format = format;
    width = w;
    height = h;
    depth = d;
    glGenTextures(1,&handle);
    glBindTexture(type,handle);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,0,format,w,h,d,0,sf,GL_UNSIGNED_BYTE,s);
    return ME_ENO_ERROR;
}

int Texture::ClearGLTexture(unsigned int start_off_contained,unsigned int end_off_contained,float r,float g,float b,float a){
    if(!handle){
        return ME_ENO_DATA;
    }
    unsigned int ret = ME_ENO_ERROR;
    glBindFramebuffer(GL_FRAMEBUFFER,frameBuffer);
    glBindTexture(type,handle);
    if(type == GL_TEXTURE_2D){
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,type,handle,0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            Util::InvokeConsole("Failed to load frame buffer!",false,"Texture",0);
            return ME_EOPENGL_ERROR;
        }
        glClearColor(r,g,b,a);
        glClear(GL_COLOR_BUFFER_BIT);
    }else if(type == GL_TEXTURE_2D_ARRAY){
        for(unsigned int idx = min(depth-1,start_off_contained);idx <= min(depth-1,end_off_contained);++idx){
            //glFramebufferTexture3D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,type,handle,0,idx);//Available to run in AMD
            glFramebufferTextureLayer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,handle,0,idx);///TODO:Why????This line is ok,while the line above is unavailable to run in Intel
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
                Util::InvokeConsole("Failed to load frame buffer!",false,"Texture2DArray",1);
                ret = ME_EOPENGL_ERROR;
                continue;
            }
            glClearColor(r,g,b,a);
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }
    ///Reverse to the origin
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return ret;
}

int Texture::UpdateGLTexture(unsigned char * bytes,unsigned int depth_offset,unsigned int w,unsigned int h,unsigned int depth,GLuint format,unsigned int alignValue,bool clearTex){
    if(clearTex){
        ClearGLTexture(depth_offset,depth_offset);
    }
    if(type == GL_TEXTURE_2D){
        if(!bytes){
            bytes = data;
            w = width;
            h = height;
        }
        glBindTexture(type,handle);
        glPixelStorei(GL_UNPACK_ALIGNMENT,alignValue);
        glTexImage2D(type,0,format,w,h,0,format,GL_UNSIGNED_BYTE,bytes);
        glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    }else if(type == GL_TEXTURE_2D_ARRAY){
        if(!bytes){
            ME_SIV("Empty data to load a texture array![this warning's once,so there maybe many warnings like this.]",0);
            return ME_ENO_DATA;
        }
        glBindTexture(type,handle);

        glPixelStorei(GL_UNPACK_ALIGNMENT,alignValue);
        glTexSubImage3D(type,0,0,0,depth_offset,w,h,depth,format,GL_UNSIGNED_BYTE,bytes);
        glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    }
    return ME_ENO_ERROR;
}
