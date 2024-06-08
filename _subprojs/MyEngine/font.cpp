#include "com/font.h"
#include <alib/astring.h>

using namespace std;
using namespace me;

bool me::MemFont::inited = false;
FT_Library me::MemFont::library = NULL;
FT_Matrix me::MemFont::italicMatrix;

MemFont::MemFont(unsigned int font_size,unsigned int def_atrribute,unsigned int bs){
    attribute = def_atrribute;
    this->font_sizexy = font_size;

    face = NULL;
    bold_strengthxy = bs;

    ///Init Library
    if(!MemFont::inited){
        MemFont::inited = true;
        FT_Error err = FT_Init_FreeType(&library);
        if(err){
            ME_SIV("Freetype has failed to be inited!",0);
            MemFont::inited = false;
        }else{
            atexit([]{
                FT_Done_FreeType(library);
            });
            italicMatrix.xx = 1 << 16;
            italicMatrix.xy = 0x5800;
            italicMatrix.yx = 0;
            italicMatrix.yy = 1 << 16;
        }
    }
}

MemFont::~MemFont(){
    if(face){
        FT_Done_Face(face);
    }
}

int MemFont::LoadFromFile(const char * fp,unsigned int face_index){
    if(!inited){
        ME_SIV("Freetype hasn't been inited!",0);
        return ME_EFONT_BAD_FREETYPE;
    }
    if(!fp){
        ME_SIV("empty file path",0);
        return ME_ENO_DATA;
    }
    ///release old face
    if(face)FT_Done_Face(face);
    face = NULL;
    FT_Error err = FT_New_Face(library,fp,face_index,&face);
    if(err)return err;

    SetFormat(ME_FONT_ATTR_PARENT);
    FT_Select_Charmap(face,FT_ENCODING_GB2312);
    return ME_ENO_ERROR;
}

int MemFont::LoadFromMem(unsigned char * buffer,unsigned long size,unsigned int face_index){
    if(!inited){
        ME_SIV("Freetype hasn't been inited!",0);
        return ME_EFONT_BAD_FREETYPE;
    }
    if(!buffer){
        ME_SIV("empty data",0);
        return ME_ENO_DATA;
    }
    if(face)FT_Done_Face(face);
    face = NULL;
    FT_Error err = FT_New_Memory_Face(library,buffer,size,face_index,&face);
    if(err)return err;

    SetFormat(ME_FONT_ATTR_PARENT);

    FT_Select_Charmap(face,FT_ENCODING_GB2312);
    return ME_ENO_ERROR;
}

void MemFont::SetFormat(unsigned int att){
    if(att | ME_FONT_ATTR_PARENT)att = attribute;
    FT_Set_Pixel_Sizes(face,font_sizexy & ME_FONTSIZE_MASK,font_sizexy >> ME_FONTSIZE_OFFSET);
}

void MemFont::SetDefSize(unsigned short font_sizex,unsigned short font_sizey){
    this->font_sizexy = ME_FONTSIZE(font_sizex,font_sizey);
}

void MemFont::SetDefAttribute(unsigned int attribute){
    ///TODO:italic is processed in shader rather than here
    this->attribute = attribute;
}

void MemFont::SetDefBoldStrength(unsigned short bx,unsigned short by){
    bold_strengthxy = ME_BOLD(bx,by);
}

FT_GlyphSlot MemFont::LoadChar(FT_ULong charcode_gbk,unsigned int attri,FT_Render_Mode_ mode,bool render){
    if(!face){
        ME_SIV("Empty face",0);
        return NULL;
    }
    FT_Load_Char(face,charcode_gbk,FT_LOAD_DEFAULT);
    SetFormat(attri);
    if(attri & ME_FONT_ATTR_PARENT)attri = attribute;
    if(attri & ME_FONT_ATTR_BOLD){
        FT_Outline_EmboldenXY(&(face->glyph->outline),bold_strengthxy & ME_BOLD_MASK,bold_strengthxy >> ME_BOLD_OFFSET);
    }
    if(attri & ME_FONT_ATTR_ITALIC){
        FT_Outline_Transform(&(face->glyph->outline),&italicMatrix);
    }
    if(render){
        FT_Render_Glyph(face->glyph,mode);
    }
    SetFormat(attribute);
    return face->glyph;
}

FT_GlyphSlot MemFont::LoadCharEx(FT_ULong charcode_gbk,unsigned int font_sizexy,unsigned int attri,FT_Render_Mode_ mode,bool render){
    if(!face){
        ME_SIV("Empty face",0);
        return NULL;
    }
    FT_UInt index = FT_Get_Char_Index(face,charcode_gbk);
    FT_Load_Glyph(face,index,FT_LOAD_DEFAULT);
    unsigned int oldxy = this->font_sizexy;
    this->font_sizexy = font_sizexy;
    SetFormat(attri);
    if(attri & ME_FONT_ATTR_PARENT)attri = attribute;
    if(attri & ME_FONT_ATTR_BOLD){
        FT_Outline_EmboldenXY(&(face->glyph->outline),bold_strengthxy & ME_BOLD_MASK,bold_strengthxy >> ME_BOLD_OFFSET);
    }
    if(attri & ME_FONT_ATTR_ITALIC){
        FT_Outline_Transform(&(face->glyph->outline),&italicMatrix);
    }
    if(render){
        FT_Render_Glyph(face->glyph,mode);
    }
    this->font_sizexy = oldxy;
    SetFormat(attribute);
    return face->glyph;
}

void MemFont::GenChar(FT_Glyph & target,FT_ULong charcode_gbk,unsigned int attri,FT_Render_Mode_ mode,bool render){
    FT_GlyphSlot glyph = LoadChar(charcode_gbk,attri,mode,render);
    if(glyph)FT_Get_Glyph(glyph,&target);
}

///GlFont
GlFont::GlFont(MemFont & mem,unsigned int width,unsigned int height,
               unsigned int depth,unsigned int font_sizexy,
               unsigned int def_atrribute,unsigned int bold_strengthxy)
        :memfont(mem){
    this->width = width;
    this->height = height;
    this->depth = depth;
    charcodes_gb = frequencies = 0;
    defGlyph = NULL;
}

inline void GlFont::SetSize(unsigned short x,unsigned short y){memfont.SetDefSize(x,y);};
inline void GlFont::SetBoldStrength(unsigned short bx,unsigned short by){memfont.SetDefBoldStrength(bx,by);}
inline void GlFont::SetAttribute(unsigned int att){memfont.SetDefAttribute(att);}

int GlFont::SetBufferSize(unsigned int w,unsigned int h,unsigned int d){
    if(buffer.handle){
        ME_SIV("Texture buffer exists now!",0);
        return ME_EALREADY_EXI;
    }
    width = w;
    height = h;
    depth = d;
    return ME_ENO_ERROR;
}

int GlFont::CreateBuffer(){
    if(!width || !height || !depth){
        ME_SIV("The target you created is empty!",0);
        return ME_ENO_DATA;
    }
    buffer.Create2DTextureArray(width,height,depth);
    charcodes_gb = new FT_ULong[depth];
    frequencies = new unsigned long[depth];
    attributes = new unsigned int[depth];

    memset(charcodes_gb,0,sizeof(FT_ULong) * depth);
    memset(frequencies,0,sizeof(unsigned long) * depth);
    memset(attributes,0,sizeof(unsigned int) * depth);

    div_line_permanent = -1;
    return ME_ENO_ERROR;
}

GlFont::~GlFont(){
    if(charcodes_gb){
        delete [] charcodes_gb;
    }
    if(frequencies){
        delete [] frequencies;
    }
    if(attributes){
        delete [] attributes;
    }
}

unsigned int GlFont::LoadCharGB2312(FT_ULong charcode_gb){
    ///Step1:search current usable
    bool hasFound = false;
    unsigned int index = 0;
    unsigned int nearest_free_space = depth;
    unsigned int proper_low_freq = depth;//freq = FREQ_LOW
    if(!buffer.handle){
        ME_SIV("Buffer haven't been created",0);
        return UINT_MAX;
    }
    ///TODO:OpenMP
    //unsigned int max_th = omp_get_max_threads();
    //#pragma omp parallel for num_threads(max_th)
    for(unsigned int i = 0;i < depth;++i){
        if(hasFound)break;
        ///Compatitable
        ///Normally ME_FONT_ATTR_PARENT will not occur
        if(charcodes_gb[i] == charcode_gb && (attributes[i] & ~ME_FONT_ATTR_PERMANENT) == (memfont.attribute & ~ME_FONT_ATTR_PERMANENT)){
            index = i;
            hasFound = true;
            break;
        }
        if(!charcodes_gb[i] && i < nearest_free_space){
            nearest_free_space = i;
        }
        ///TODO:unstable
        if(attributes[i]&ME_FONT_ATTR_PERMANENT && frequencies[i] >> 5  && frequencies[i] < (proper_low_freq==depth?UINT_MAX:frequencies[proper_low_freq])){
            proper_low_freq = i;
        }
    }
    if(hasFound){
        ++frequencies[index];
        return index;
    }else{
        ///Create A new object
        unsigned int dv = nearest_free_space;
        if(dv == depth){
            ///check frequencies
            if(proper_low_freq == depth){
                ME_SIV("Oppps,the frequencies aren;t bigger,replace the last usable element.I don't care if it is perm or not.",1);
                proper_low_freq = depth-1;
            }
            _FreeCharData(proper_low_freq);
            _LoadCharData(charcode_gb,proper_low_freq);
            _UpdateOpenGL(proper_low_freq);
            ++frequencies[proper_low_freq];
            return proper_low_freq;
        }else{
            _LoadCharData(charcode_gb,dv);
            _UpdateOpenGL(dv);
            ++frequencies[dv];
            return dv;
        }
    }
}

void GlFont::_FreeCharData(unsigned int index){
    charcodes_gb[index] = 0;
    frequencies[index] = 0;
    attributes[index] = 0;
}

void GlFont::_LoadCharData(FT_ULong code,unsigned int index){
    charcodes_gb[index] = code;
    frequencies[index] = 0;
    attributes[index] = memfont.attribute;
    defGlyph = memfont.LoadChar(code,ME_FONT_ATTR_PARENT);
}

void GlFont::_UpdateOpenGL(unsigned int index){
    buffer.UpdateGLTexture(defGlyph->bitmap.buffer,index,defGlyph->bitmap.width,defGlyph->bitmap.rows,1,GL_RED,1,true);
}

unsigned int GlFont::LoadCharUnicode(FT_ULong charcode_un){
    wstring wstr = L"";
    wstr += charcode_un;
    return LoadCharGB2312(alib::ng::converter::utf16_to_ansi(wstr)[0]);
}

unsigned int GlFont::LoadCharUTF8(FT_ULong charcode_u8){
    string str = "";
    str += charcode_u8;
    return LoadCharGB2312(alib::ng::converter::utf8_to_ansi(str)[0]);
}

///CharProperty hash
 size_t std::hash<CharProperty>::operator()(const CharProperty& cp) const {
    // 使用charcode的哈希值作为基础
    size_t hashValue = std::hash<FT_ULong>()(cp.charcode);
    // 混合其他成员的哈希值
    hashValue ^= std::hash<unsigned int>()(cp.attribute) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    hashValue ^= std::hash<unsigned int>()(cp.bold_strength) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    hashValue ^= std::hash<unsigned int>()(cp.font_size) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    return hashValue;
}

///fGlFont
fGlFont::fGlFont(MemFont & mft,unsigned int allocate):memfont(mft){
    buffer.CreateTextureBuffer(allocate);
    bufferInCPU = new unsigned char[allocate];
}

unsigned int fGlFont::LoadCharGB2312(FT_ULong charcode){
    return LoadCharGB2312Ex(charcode,48,ME_FONT_ATTR_PARENT,8).offset;
}

CharData fGlFont::LoadCharGB2312Ex(FT_ULong charcode,unsigned int font_size,unsigned int attri,unsigned int bold_strenth){
    return {};
}

fGlFont::~fGlFont(){
    if(bufferInCPU)delete bufferInCPU;
}


