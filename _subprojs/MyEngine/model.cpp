#include <com/model.h>
#include <memory>
#include <sstream>
#include <fstream>

using namespace std;
using namespace me;

int Model::LoadModelFromObj(const char * fname){
    unique_ptr<ObjLoader> obj;
    obj.reset(new ObjLoader(vfloats,nfloats,tfloats,vertc));
    return obj->LoadFromObj(fname)?ME_ENO_ERROR:ME_EBAD_IO;
}

int Model::LoadModelFromStlBin(const char * fname){
    unique_ptr<ObjLoader> obj;
    obj.reset(new ObjLoader(vfloats,nfloats,tfloats,vertc));
    return obj->LoadFromStlBin(fname)?ME_ENO_ERROR:ME_EBAD_IO;
}

void Model::CreateVBOs(VBO&vvbo,VBO& nvbo){
    vbo = vvbo;
//    ivbo = vbo1;
//    ivbo.vbo_type = ME_VBO_ELEMENT;
    this->nvbo = nvbo;
}

void Model::Unbind(){
    vbo.unbind();
//    ivbo.unbind();
    nvbo.unbind();
}

void Model::UploadToOpenGL(){
    if(!vbo.GetVBO()){
        ME_SIV("Some vbos are unavailable!",0);
        return;
    }
    vbo.Set(&(vfloats[0]),vfloats.size() * sizeof(float));
//    ivbo.Set(&(indices[0]),indices.size() * sizeof(float));
    if(nfloats.size() > 0){
        hasNormal = true;
        nvbo.Set(&(nfloats[0]),nfloats.size()*sizeof(float));
    }
}

void Model::SetBindings(GLuint v,GLuint v1){
    vbo.bind2(v);
//    ivbo.bind();
    if(hasNormal){
        nvbo.bind2(v1);
    }
}

void Model::SetData(vector<float>* vc,vector<float>* texc,vector<float>* norm){
    if(!vc & !texc & !norm){ME_SIV("empty call",0);return;}
    if(vc)vfloats = *vc;
    if(texc)tfloats = *texc;
    if(norm)nfloats = *norm;
}

Model::Model(float x,float y,float z){
    GObject(x,y,z);
    hasNormal = false;
}

///Model1 ?


///ObjLoader
///TODO ME_SIV!!!! When return false
///ObjLoader
bool ObjLoader::LoadFromObj(const char * obj_path){
   char token;
   string rest;
   glm::vec3 v;
   string mtl = "";
   vertc = 0;
   ///Load Obj
   if(!obj_path)return false;
   stringstream fobj;
   ifstream ffobj(obj_path);
   if(ffobj.bad())return false;
   //Read all the data
   {
     ffobj.seekg(0,ios::end);
     int sz = ffobj.tellg();
     ffobj.clear();
     ffobj.seekg(0,ios::beg);
     char * buf = new char[sz+1];
     memset(buf,0,sizeof(char) * (sz+1));
     ffobj.read(buf,sz);
     fobj.str(buf);
     delete [] buf;
     ffobj.close();
   }

   while(!fobj.eof()){
    fobj >> token;
    switch(token){
    //vertices
    case 'v':
        v.x = v.y = v.z = 0;
        fobj.get(token);
        //fobj >> token; 用符号>>会跳过空白 use operator ">>" would skip spaces
        if(token == ' '){
            fobj >> v.x >> v.y >> v.z;
//            cout << "V:" << v.x << " " << v.y << " " << v.z << endl;
            vertices.push_back(v);
        }else if(token == 'n'){
            fobj >> v.x >> v.y >> v.z;
//            cout << "N:" << v.x << " " << v.y << " " << v.z << endl;
            normals.push_back(v);
        }else if(token == 't'){
            fobj >> v.x >> v.y;
//            cout << "T:" << v.x << " " << v.y << endl;
            tcoords.push_back(glm::vec2(v.x,v.y));
        }
        break;
    //faces
    case 'f':{
        ///注意:face的所有起始索引为1而非0，注意对齐
        int index = 0;
        vertc += 3;
        for(int xx = 0;xx < 3;++xx){
            ///原来顺序是vtn,vertex/texcoord/normal 气死我了！
            fobj >> index;
            if(index != 0){
//                vindices.push_back(index-1);
                vfloats.push_back(vertices[index-1].x);
                vfloats.push_back(vertices[index-1].y);
                vfloats.push_back(vertices[index-1].z);
            }
            fobj.get(token);
            if(token != '/')continue;

            fobj >> index;
            ///缺点：f标签必须在v,vt,vn标签之后
            if(index != 0){
//                tindices.push_back(index-1);
                vtexc.push_back(tcoords[index-1].x);
                vtexc.push_back(tcoords[index-1].y);
            }
            fobj.get(token);
            if(token != '/')continue;

            fobj >> index;
            if(index != 0){
//                nindices.push_back(index-1);
                vnormals.push_back(normals[index-1].x);
                vnormals.push_back(normals[index-1].y);
                vnormals.push_back(normals[index-1].z);
            }
            //cout << "FACE:" << vindices[vindices.size()-1] << " "
            //<< nindices[nindices.size()-1] << " "
            //<< tindices[tindices.size()-1] << " " << endl;
        }
        break;
    }
//    case 'u':
//        fobj >> rest;
//        if(rest.compare("semtl"))break;
//        ///TODO:USE MTL
//        break;
//    case 'm':
//        fobj >> rest;
//        if(rest.compare("tllib"))break;
//        ///TODO:MTL LIB
//        break;
    default:
        getline(fobj,rest);
        //cout << "Skipped:" << token << rest << endl;
        break;
    }
   }
   ///TODO:READ MTL
   return true;
}

bool ObjLoader::LoadFromStlBin(const char * fp){
    if(!fp)return false;
    FILE * file = fopen(fp,"rb");
    if(!file)return false;
    ///Skip the first eighty chars
    fseek(file,80,SEEK_SET);
    vertc = 0;
    fread(&vertc,4,1,file);
    float v;
    for(unsigned int i = 0;i < vertc;++i){
        ///Normal
        v = 0;
        for(unsigned int ix = 0;ix < 3;++ix){
            fread(&v,4,1,file);
            vnormals.push_back(v);
        }
        ///repeat for 2 times
        for(unsigned int ix = 0;ix < 3*2;++ix){
            vnormals.push_back(vnormals[vnormals.size()-3]);
        }
        ///vertices
        for(unsigned int iy = 0;iy < 3;++iy){
            v = 0;
            for(unsigned int ix = 0;ix < 3;++ix){
                fread(&v,4,1,file);
                vfloats.push_back(v);
            }
        }
        fread(&v,2,1,file);
    }
    //給的是面的數量
    vertc *= 3;
    fclose(file);
    return true;
}

ObjLoader::ObjLoader(vector<float>&vf,vector<float>& nm,vector<float>& tc,unsigned int& fc):
    vfloats(vf),vnormals(nm),vtexc(tc),vertc(fc){}
