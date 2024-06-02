#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED

///macros
//arg pass
#define ME_DETECT_SIZE 0

//Errors
#define ME_ENO_ERROR                          0b0
#define ME_EOPENGL_ERROR                      0b1
#define ME_EEMPTY_STRING                      0b10
#define ME_EBAD_IO                            0b100
#define ME_EBAD_MEM                           0b1000
#define ME_EBAD_TYPE                          0b10000
#define ME_ENO_DATA                           0b100000
#define ME_EALREADY_EXI                       0b1000000
#define ME_EFONT_BAD_FREETYPE                 0b10000000
//shaders
#define ME_SHADER_VERTEX GL_VERTEX_SHADER
#define ME_SHADER_FRAGMENT GL_FRAGMENT_SHADER
#define ME_SHADER_GEOMETRY GL_GEOMETRY_SHADER
#define ME_SHADER_TYPEC 3 //available shader types
//vbos
#define ME_VBO_ELEMENT GL_ELEMENT_ARRAY_BUFFER
#define ME_VBO_VERTEX  GL_ARRAY_BUFFER
//implement opengl
#define ME_CW GL_CW
#define ME_CCW GL_CCW
//fonts
#define ME_FONT_ATTR_PARENT      0b1
#define ME_FONT_ATTR_ITALIC      0b10
#define ME_FONT_ATTR_BOLD        0b100
#define ME_FONT_ATTR_UNDERLINE   0b1000
#define ME_FONT_ATTR_DELETELINE  0b10000
#define ME_FONT_ATTR_PERMANENT   0b100000

///Tools
#define ME_BOLD_OFFSET 16
#define ME_BOLD(X,Y) (unsigned int)((Y << ME_BOLD_OFFSET) + X)
#define ME_BOLD_MASK 0xffff

#define ME_FONTSIZE_OFFSET 16
#define ME_FONTSIZE(X,Y) (unsigned int)((Y << ME_FONTSIZE_OFFSET) + X)
#define ME_FONTSIZE_MASK 0xffff

#define ME_FONT_OUTLINE_NUM_POINTS 256
#define ME_FONT_OUTLINE_NUM_CONTOURS 128

//ignore 0 --- 31
#define ME_FREQ_LOW 5

///simple out of util
#ifdef DEBUG
#define ME_SIV(msg,rsg) Util::InvokeConsole(msg,true,__FUNCTION__,(long long)this + rsg)
#define ME_SIVD(msg,rsg) Util::InvokeConsole(msg,true,__FUNCTION__,rsg)
#else
#define ME_SIV(msg,rsg)
#define ME_SIVD(msg,rsg)
#endif // DEBUG

///OpenGL supports
#define EXT_AF "GL_EXT_texture_filter_anisotropic"

#define ME_FRAME_ADJUST_V 0.8

#ifndef PI
    #define PI 3.1415926
#endif // PI

#ifndef rad2deg
    #define rad2deg(r) (180*((r)/PI))
#endif // rad2deg

#ifndef deg2rad
    #define deg2rad(d) (((d)/180)*PI)
#endif // deg2rad


#endif // MACROS_H_INCLUDED
