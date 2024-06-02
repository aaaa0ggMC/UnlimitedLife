#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED
#include <com/utility.h>
#include <com/texture.h>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_IMAGE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

namespace me{
    struct fGlFont;
    struct CharProperty;
}
namespace std {
    template <>
    struct hash<me::CharProperty> {
        size_t operator()(const me::CharProperty& cp) const;
    };
}

namespace me{
    using namespace std;
    class MemFont : public noncopyable{
    public:
        static FT_Library library;
        static bool inited;
        static FT_Matrix italicMatrix;

        FT_Face face;

        ///Helpers
        unsigned int step;

        unsigned int attribute,font_sizexy;
        unsigned int bold_strengthxy;

        MemFont(unsigned int font_sizexy = ME_FONTSIZE(0,48),unsigned int def_atrribute = 0,unsigned int bold_strengthxy = ME_BOLD(128,128));
        ~MemFont();

        int LoadFromFile(const char * filePath,unsigned int face_index = 0);
        int LoadFromMem(unsigned char * buffer,unsigned long size,unsigned int face_index= 0);

        FT_GlyphSlot LoadChar(FT_ULong charcode_gbk,unsigned int attri = ME_FONT_ATTR_PARENT,FT_Render_Mode_ mode = FT_RENDER_MODE_NORMAL,bool render = true);
        FT_GlyphSlot LoadCharEx(FT_ULong charcode_gbk,unsigned int font_sizexy,unsigned int attri = ME_FONT_ATTR_PARENT,FT_Render_Mode_ mode = FT_RENDER_MODE_NORMAL,bool render = true);

        void GenChar(FT_Glyph & target,FT_ULong charcode_gbk,unsigned int attri = ME_FONT_ATTR_PARENT,FT_Render_Mode_ mode = FT_RENDER_MODE_NORMAL,bool render = true);

        void SetDefSize(unsigned short font_sizex,unsigned short font_sizey);
        void SetDefAttribute(unsigned int attribute);
        void SetDefBoldStrength(unsigned short bx,unsigned short by);

        void SetFormat(unsigned int att);
    };

    ///CharProperty
    struct CharProperty{
        FT_ULong charcode;
        unsigned int attribute;
        unsigned int bold_strength;
        unsigned int font_size;
    };
    ///Char Data
    struct CharData{
        unsigned int offset;
        unsigned int width;
        unsigned int height;
    };

    ///fGlFont
    class fGlFont : public noncopyable{
    public:
        MemFont & memfont;
        Texture buffer;
        unsigned char * bufferInCPU;

        unordered_map<CharProperty,CharData> charcodes;

        unsigned int allocate;



        ///returns offset
        unsigned int LoadCharGB2312(FT_ULong charcode);
        CharData LoadCharGB2312Ex(FT_ULong charcode,unsigned int font_size,unsigned int attri,unsigned int bold_strenth);


        fGlFont(MemFont&,unsigned int allocate);
        ~fGlFont();
    };

    class GlFont : public noncopyable{
    public:
        MemFont& memfont;
        Texture buffer;
        FT_ULong * charcodes_gb;
        unsigned long * frequencies;
        unsigned int * attributes;
        FT_GlyphSlot defGlyph;


        unsigned int width,height,depth;
        ///a index that determines the permanence of the elements,id bigger than this will be detected
        int div_line_permanent;


        GlFont(MemFont& memfont,unsigned int width = 0,unsigned int height = 0,unsigned int depth = 0,unsigned int font_sizexy = ME_FONTSIZE(0,48),unsigned int def_atrribute = 0,unsigned int bold_strengthxy = ME_BOLD(4,4));
        ~GlFont();

        void SetSize(unsigned short font_sizex,unsigned short font_sizey);
        void SetAttribute(unsigned int attribute);
        void SetBoldStrength(unsigned short bx,unsigned short by);
        int SetBufferSize(unsigned int w,unsigned int h,unsigned int d);

        int CreateBuffer();

        unsigned int LoadCharGB2312(FT_ULong charcode_gb);
        unsigned int LoadCharUnicode(FT_ULong charcode_un);
        unsigned int LoadCharUTF8(FT_ULong charcode_u8);

        void _LoadCharData(FT_ULong charcode_gb,unsigned int index);
        void _FreeCharData(unsigned int index);
        void _UpdateOpenGL(unsigned int index);

        ///Release all frequencies
        void ReleaseCache();
    };
}

#endif // FONT_H_INCLUDED
