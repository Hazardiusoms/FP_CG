#include "Hud.hpp"
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
namespace sw {
namespace {
// Original 5-by-7 bitmap glyphs, expanded into triangles; no font dependency.
const std::map<char,std::string> glyphs={
{'A',"0E11111F111111"},{'B',"1E11111E11111E"},{'C',"0F10101010100F"},
{'D',"1E11111111111E"},{'E',"1F10101E10101F"},{'F',"1F10101E101010"},
{'G',"0F10101711110F"},{'H',"1111111F111111"},{'I',"1F04040404041F"},
{'J',"0702020212120C"},{'K',"11121418141211"},{'L',"1010101010101F"},
{'M',"111B1515111111"},{'N',"11191513111111"},{'O',"0E11111111110E"},
{'P',"1E11111E101010"},{'Q',"0E11111115120D"},{'R',"1E11111E141211"},
{'S',"0F10100E01011E"},{'T',"1F040404040404"},{'U',"1111111111110E"},
{'V',"11111111110A04"},{'W',"11111115151B11"},{'X',"11110A040A1111"},
{'Y',"11110A04040404"},{'Z',"1F01020408101F"},
{'0',"0E11131519110E"},{'1',"040C040404040E"},{'2',"0E11010204081F"},
{'3',"1E01010E01011E"},{'4',"02060A121F0202"},{'5',"1F10101E01011E"},
{'6',"0E10101E11110E"},{'7',"1F010204080808"},{'8',"0E11110E11110E"},
{'9',"0E11110F01010E"},{':',"00040400040400"},{'.',"00000000000C0C"},
{'/',"01020204080810"},{'-',"0000001F000000"},{'|',"04040404040404"}};
void rect(std::vector<float>& data,float x,float y,float w,float h,Vec3 color){
    for(auto v : {Vec3{x,y,0},Vec3{x+w,y,0},Vec3{x+w,y+h,0},Vec3{x,y,0},Vec3{x+w,y+h,0},Vec3{x,y+h,0}})
        data.insert(data.end(),{v.x,v.y,color.x,color.y,color.z});
}
void text(std::vector<float>& data,const std::string& value,float x,float y,float scale,Vec3 color){
    for(char c:value){
        const auto it=glyphs.find(c);
        if(it!=glyphs.end())for(int row=0;row<7;++row){
            const int bits=std::stoi(it->second.substr(static_cast<std::size_t>(row)*2,2),nullptr,16);
            for(int col=0;col<5;++col)if(bits&(1<<(4-col)))
                rect(data,x+col*scale,y+row*scale,scale,scale,color);
        }
        x+=6*scale;
    }
}
}
void Hud::initialize(const std::filesystem::path& shaders){
    shader_.initialize(shaders/"hud.vert",shaders/"hud.frag");
    glGenVertexArrays(1,&vao_);glGenBuffers(1,&vbo_);glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glEnableVertexAttribArray(0);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,5*sizeof(float),nullptr);
    glEnableVertexAttribArray(1);glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,5*sizeof(float),reinterpret_cast<void*>(2*sizeof(float)));
    glBindVertexArray(0);
}
void Hud::render(const RenderContext& ctx,const SceneState& scene,double fps){
    std::vector<float> data;
    const float scale=ctx.framebufferWidth<900 ? 1.0f : 2.0f;
    std::ostringstream status;
    status<<"FPS: ";if(fps>0)status<<std::fixed<<std::setprecision(1)<<fps;else status<<"--";
    status<<" | SPEED: "<<static_cast<int>(scene.speed)<<" | "<<(scene.paused?"PAUSED":"RUNNING")
          <<" | "<<(scene.camera==CameraMode::Follow ? "FOLLOW":"ORBIT");
    rect(data,10,10,std::min(790.0f*scale/2,static_cast<float>(ctx.framebufferWidth)-20),scene.showHelp ? 132*scale/2 : 24*scale,{0.025f,0.03f,0.065f});
    text(data,status.str(),20,20,scale,{0.3f,0.9f,1});
    if(scene.showHelp){
        text(data,"W/S SPEED | SPACE PAUSE | G PALETTE",20,20+12*scale,scale,{0.9f,0.9f,0.95f});
        text(data,"C CAMERA | I/J/K/L ORBIT | U/O ZOOM",20,20+24*scale,scale,{0.9f,0.9f,0.95f});
        text(data,"RIGHT MOUSE DRAG ORBIT | SCROLL ZOOM",20,20+36*scale,scale,{0.9f,0.9f,0.95f});
        text(data,"H HELP | R RESET | ESC EXIT",20,20+48*scale,scale,{0.9f,0.9f,0.95f});
    }
    text(data,"FRAMEWORK PREVIEW - CAR AND ENVIRONMENT PENDING",20,ctx.framebufferHeight-20.0f,scale,{0.65f,0.65f,0.78f});
    const bool depth=glIsEnabled(GL_DEPTH_TEST);GLboolean write;glGetBooleanv(GL_DEPTH_WRITEMASK,&write);
    glDisable(GL_DEPTH_TEST);glDepthMask(GL_FALSE);
    shader_.use();shader_.set("uViewport",Vec3{float(ctx.framebufferWidth),float(ctx.framebufferHeight),0});
    glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,data.size()*sizeof(float),data.data(),GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(data.size()/5));glBindVertexArray(0);
    if(depth)glEnable(GL_DEPTH_TEST);glDepthMask(write);
}
void Hud::shutdown(){if(vbo_)glDeleteBuffers(1,&vbo_);if(vao_)glDeleteVertexArrays(1,&vao_);vao_=vbo_=0;shader_.shutdown();}
}
