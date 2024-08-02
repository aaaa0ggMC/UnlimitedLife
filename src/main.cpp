#include <ME.h>
#include <alib/autil.h>
#include <alib/aclock.h>
#include <alib/astring.h>
#include <alib/alogger.h>
#include <alib/adata.h>
#include <toml.hpp>
#include <sol/sol.hpp>
#include "-terrarian/Chunk.h"
#include "reses.h"
#define numVBOs 32
#define numVAOs 1

using namespace std;
using namespace me;
using namespace alib;

///general
string resbeg = "res";
Velocity camSpeed(10);
ng::Clock clk(false);
Window window;
Program game;
ng::Logger loggerSaver;
ng::LogFactory logger("UnlimitedLife",loggerSaver);

///gameobjects
Camera c(0,0,6);
GObject vcx(0,0,0),cube(0,-5,-5);

///textures
bool reloadTag = false;
unsigned int offset = 0;
const unsigned int maxium = 512;
Texture test,test3d;

///shaders
Shader shader(false);
sol::state shader_inter;

///fonts
MemFont testFont;
GlFont glfont(testFont);

///opengl
VBOs vbo;
GLuint vao[numVAOs];

void init();
void paint(Window& w,double currentTime,Camera*cam);
void input(Window& w,double elapseus,Camera * c);
int real_main();


int main()
{
    try{
        return real_main();
    }catch(...){
        MessageBox(NULL,"Exception Appeared!","ERROR",MB_OK | MB_ICONERROR | MB_TOPMOST);
    }
    return -1;
}

int real_main(){
    ng::GDoc doc;

    ///初始化Logger
    Util::SetLogger(loggerSaver);
    loggerSaver.setOutputFile("data/logs/latest.log");

    ///初始化窗口
    MEQuickCreateWindow(window,800,600,"UnlimitedLife",paint,input,c);
    MEApplyCameraPerspecAndReSize(window,c,60,0.1,1000);
    /*(c).SetPerspec(1.0f,0.1,1000);
    (c).BuildPerspec(&window);
    (window).OnResize([](Window&win,int nw,int nh){
        win.SetViewport(0,0,nw,nh);\
        if(win.curCam)win.curCam->Build(&window);
    });*/


    int ret = doc.read_parseFileTOML(URES_OPTIONS);
    if(!ret){
        auto data = doc["res_path"];
        if(!!data){
            resbeg = *data;
        }
    }else{
        ///TODO:错误处理
        logger << "Failed to read \"" URES_OPTIONS "\",use default values.Error code:" << ret << alib::ng::endlog;
    }

    init();
    clk.start();
    while(!window.ShouldClose()){
        window.MakeCurrent();
        window.PollEvents();
        window.Display();
    }
    window.Destroy();
    return 0;
}

void paint(Window& w,double currentTime,Camera*c){
    static ShaderArg m_mat = ShaderArg(shader.GetProgram(),1);
    static ShaderArg vrp = ShaderArg(shader.GetProgram(),2);
    static ng::Trigger trigger_title_update(clk,100);

    window.Clear();

    c->Update();
    game.Update();

    //text.Activate(0);

    //glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D_ARRAY,test3d);
    //test3d.Activate(0);
    glfont.buffer.Activate(0);

    glDisable(GL_CULL_FACE);

    shader.bind();
    vrp = c->vrp_matrix;

    glFrontFace(ME_CCW);
    m_mat = vcx.mat;
    w.Draw(vcx,6,maxium);

    ///
    glFrontFace(ME_CW);
    m_mat = cube.mat;
	w.Draw(cube,36);
	shader.unbind();

	///fps display
    static char buf[48];
    static unsigned int smfps = 0;
    static unsigned int all = 0;
    if(trigger_title_update.test()){
        //trigger_title_update.reset(); test默认自动执行reset
        memset(buf,0,sizeof(char) * 48);
        sprintf(buf,
                "HiGLEW-% .2ffps | average %.2ffps | % .2fmspf",
                1000.0/trigger_title_update.duration * smfps,
                1000 * all / clk.getAllTime(),
                trigger_title_update.duration / (float)smfps);
        SetWindowText((HWND)(long long)window.GetSystemHandle(),buf);
        smfps = 0;
        if(reloadTag){
            reloadTag = false;
            //testFont.SetDefAttribute(ME_FONT_ATTR_ITALIC);
            for(unsigned int ac = 0;ac < maxium;++ac){
                glfont.LoadCharGB2312(ac + 32 + offset);
                //FT_GlyphSlot glyph = testFont.LoadChar(ac+offset+32);
                ///source depth should be 1,or the texture would'nt be updated
                //test3d.UpdateGLTexture(glyph->bitmap.buffer,ac,glyph->bitmap.width,glyph->bitmap.rows,1,GL_RED,1,false);
                //glTexSubImage3D(GL_TEXTURE_2D_ARRAY,0,0,0,ac,glyph->bitmap.width,glyph->bitmap.rows,1,GL_RED,GL_UNSIGNED_BYTE,glyph->bitmap.buffer);
            }
            offset += maxium;
        }
    }
    smfps++;
    all++;
}

void init(){
    testFont.LoadFromFile((resbeg + "/fonts/rtest.ttf").c_str());
    glfont.SetBufferSize(64,64,maxium);
    glfont.CreateBuffer();

    glGenVertexArrays(numVAOs,vao);
    glBindVertexArray(vao[0]);

    vbo.AppendVBOs(numVBOs);

    float rect[] = {
    0,0,0,
    1,0,0,
    0,1,0,
    0,1,0,
    1,0,0,
    1,1,0
    };

    float vcoord[] = {
    0,1,
    1,1,
    0,0,
    0,0,
    1,1,
    1,0
    };

    float vertexPositions[108] = {
	 -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
	 -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
	 -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
	};

    vbo[0].Set(rect,sizeof(rect));

    vbo[1].tps = 2;
    vbo[1].Set(vcoord,sizeof(vcoord));

    vbo[3].Set(vertexPositions,sizeof(vertexPositions));

    vcx.BindVBO(vbo[0],vbo[1]);
    vcx.SetBindings(0,1);
    cube.BindVBO(vbo[3]);
    vcx.SetBindings(0,1);

    {
        string path = resbeg + URES_MAIN_SHADER_LUA;
        string data = "";
        ng::Util::io_readAll(path,data);

        shader_inter.set("vert","");
        shader_inter.set("frag","");
        shader_inter.script(data);

        string vert = shader_inter["vert"];
        string frag = shader_inter["frag"];

        shader.CreateProgram();
        shader.LoadLinkLogM(vert.c_str(),frag.c_str());
        game.PushObj({&vcx,&cube});
    }
    test.LoadFromFile((resbeg + "/imgs/sb.png").c_str());
    test.UploadToOpenGL();

    test3d.Create2DTextureArray(64,64,maxium);

    for(unsigned int ac = 0;ac < maxium;++ac){
        glfont.LoadCharGB2312(ac + offset + 32);
        //FT_GlyphSlot glyph = testFont.LoadChar(ac+offset+32);
        //test3d.UpdateGLTexture(glyph->bitmap.buffer,ac,glyph->bitmap.width,glyph->bitmap.rows,1,GL_RED,1,false);
        //glTexSubImage3D(GL_TEXTURE_2D_ARRAY,0,0,0,ac,glyph->bitmap.width,glyph->bitmap.rows,1,GL_RED,GL_UNSIGNED_BYTE,glyph->bitmap.buffer);
    }

}

void input(Window& w,double elapseus,Camera * cx){

    camSpeed.New();
    if(w.KeyInputed(GLFW_KEY_SPACE))
        camSpeed.Add(0,1,0);
    else if(w.KeyInputed(GLFW_KEY_LEFT_SHIFT))
        camSpeed.Add(0,-1,0);

    if(w.KeyInputed(GLFW_KEY_A))
        camSpeed.Add(-1,0,0);
    else if(w.KeyInputed(GLFW_KEY_D))
        camSpeed.Add(1,0,0);

    if(w.KeyInputed(GLFW_KEY_S))
        camSpeed.Add(0,0,-1);
    else if(w.KeyInputed(GLFW_KEY_W))
        camSpeed.Add(0,0,1);

    camSpeed.Form();
    camSpeed.MoveDr(*cx,elapseus);

    if(w.KeyInputed(GLFW_KEY_HOME) || w.KeyInputed(GLFW_KEY_LEFT)){
        cx->Rotate(0,deg2rad(90 * elapseus),0);
    }else if(w.KeyInputed(GLFW_KEY_END) || w.KeyInputed(GLFW_KEY_RIGHT)){
        cx->Rotate(0,deg2rad(-90 * elapseus),0);
    }

    if(w.KeyInputed(GLFW_KEY_PAGE_UP) || w.KeyInputed(GLFW_KEY_UP)){
        cx->Rotate(deg2rad(60 * elapseus),0);
    }else if(w.KeyInputed(GLFW_KEY_PAGE_DOWN) || w.KeyInputed(GLFW_KEY_DOWN)){
        cx->Rotate(deg2rad(-60 * elapseus),0);
    }

    if(glfwGetKey(w.GetGLFW(),GLFW_KEY_M) == GLFW_PRESS){
        ///血的教训：由于input由glfw异步执行，因此这里无法使用OpenGL的大部分函数！
        ///Mistake!!:Due to the async-running of input executed by glfw,most of the functions of OpenGL are unavailable
        reloadTag = true;
    }
}
