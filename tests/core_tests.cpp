#include "Camera.hpp"
#include "Input.hpp"
#include <iostream>
#include <stdexcept>
#include <cmath>
using namespace sw;
namespace {
int checks=0;
void check(bool ok,const char* message) { ++checks; if(!ok) throw std::runtime_error(message); }
bool close(float a,float b,float tolerance=1e-4f) { return std::abs(a-b)<tolerance; }
void mathTests() {
    const Mat4 model=translation({3,4,5})*rotationY(pi/2)*scaling({2,3,4});
    const Vec4 point=model*Vec4{1,0,0,1};
    check(close(point.x,3)&&close(point.y,4)&&close(point.z,3),"Transform order/column convention");
    const Vec4 child=(translation({3,0,0})*translation({0,2,0}))*Vec4{0,0,0,1};
    check(close(child.x,3)&&close(child.y,2),"Parent-child translation");
    const Mat4 view=lookAt({0,3,7},{0,0.6f,-8},{0,1,0});
    const Vec4 eye=view*Vec4{0,3,7,1};
    check(close(eye.x,0)&&close(eye.y,0)&&close(eye.z,0),"View does not map eye to origin");
    const Vec4 target=view*Vec4{0,0.6f,-8,1};
    check(close(target.x,0)&&close(target.y,0)&&target.z<0,"Camera target not on negative view Z");
    const Mat4 projection=perspective(radians(55),2,0.1f,400);
    const Vec4 nearClip=projection*Vec4{0,0,-0.1f,1};
    const Vec4 farClip=projection*Vec4{0,0,-400,1};
    check(close(nearClip.z/nearClip.w,-1)&&close(farClip.z/farClip.w,1),"Perspective depth endpoints");
    const Vec3 tangent=normalized({1,1,0}), normal=normalized({1,-1,0});
    const Vec4 transformedTangent=model*Vec4{tangent.x,tangent.y,tangent.z,0};
    const Vec3 correctedNormal=normalMatrix(model)*normal;
    check(close(dot(correctedNormal,{transformedTangent.x,transformedTangent.y,transformedTangent.z}),0),"Normal not perpendicular after non-uniform scale");
    bool rejected=false;
    try { normalMatrix(scaling({1,0,1})); } catch(const std::invalid_argument&) { rejected=true; }
    check(rejected,"Singular normal matrix accepted");
    rejected=false;
    try { perspective(radians(55),0,0.1f,100); } catch(const std::invalid_argument&) { rejected=true; }
    check(rejected,"Invalid projection accepted");
}
void stateTests() {
    SceneState slow,fast; InputState up; up.speedAdjustment=1;
    for(int i=0;i<30;++i) updateScene(slow,up,1.0f/30);
    for(int i=0;i<144;++i) updateScene(fast,up,1.0f/144);
    check(close(slow.speed,fast.speed,0.001f)&&close(slow.speed,20),"Speed depends on frame count");
    for(int i=0;i<500;++i) updateScene(slow,up,0.05f);
    check(slow.speed==maximumSpeed,"Maximum speed not clamped");
    up.speedAdjustment=-1;
    for(int i=0;i<500;++i) updateScene(slow,up,0.05f);
    check(slow.speed==0,"Speed falls below zero");
    double low=0,high=0;
    for(int i=0;i<30;++i) low=advanceGridOffset(low,7,1.0f/30);
    for(int i=0;i<144;++i) high=advanceGridOffset(high,7,1.0f/144);
    check(std::abs(low-high)<1e-5,"UV movement depends on frame count");
    check(advanceGridOffset(0,2,1)==0,"Grid period does not wrap");
    check(advanceGridOffset(0.2,0,10)==0.2,"Zero speed moves grid");
    check(advanceGridOffset(0.9,40,0.05f)<1,"Offset out of range");
    check(frameStep(-1)==0 && close(frameStep(100),0.05f),"Time-step bounds");
    slow.carX=2; slow.steeringTilt=1; slow.bloomEnabled=true;
    InputState reset; reset.reset=true; reset.changeCamera=true;
    updateScene(slow,reset,0.05f);
    check(slow.carX==0 && slow.steeringTilt==0 && slow.speed==initialSpeed && !slow.bloomEnabled && slow.camera==CameraMode::Follow,"Reset defaults/priority");
}
void inputTests() {
    Input input;
    input.set(Key::C,true); check(input.consume().changeCamera,"First camera press missing");
    input.set(Key::C,true); check(!input.consume().changeCamera,"Key repeat retriggers camera");
    input.set(Key::C,false); input.set(Key::C,true); check(input.consume().changeCamera,"Second camera press missing");
    input.set(Key::W,true); check(input.consume().speedAdjustment==1,"Held speed input missing");
    input.set(Key::S,true); check(input.consume().speedAdjustment==0,"Opposite speed keys don't cancel");
    input.set(Key::A,true); input.set(Key::Left,true); check(input.consume().steering==-1,"Equivalent steering keys double count");
    input.set(Key::Right,true); check(input.consume().steering==0,"Opposite steering keys don't cancel");
    input.clear(); check(input.consume().speedAdjustment==0&&!input.down(Key::C),"Focus-loss clear leaves held keys");
    // A fast press/release between frames must still yield its one-shot action.
    input.set(Key::R,true); input.set(Key::R,false); check(input.consume().reset,"Quick reset tap lost");
}
void cameraTests() {
    Camera camera; SceneState s; RenderContext ctx;
    check(camera.update(s,1600,900,ctx),"Camera refused valid dimensions");
    check(close(ctx.projection.at(1,1)/ctx.projection.at(0,0),1600.0f/900),"Wrong aspect ratio");
    check(close(dot(ctx.cameraForward,ctx.cameraRight),0)&&close(length(ctx.cameraUp),1),"Camera basis invalid");
    const Vec3 first=ctx.cameraPosition;
    s.camera=CameraMode::Presentation; camera.update(s,900,1600,ctx);
    check(length(ctx.cameraPosition-first)>1,"Second camera identical");
    check(!camera.update(s,0,0,ctx),"Zero framebuffer accepted");
    s=SceneState{}; s.carX=2; camera.update(s,1600,900,ctx);
    check(ctx.cameraPosition.x>0,"Follow camera ignores future car state");
}
void newControlTests(){
    SceneState a,b; InputState orbit; orbit.orbitHorizontal=1;
    for(int i=0;i<30;++i)updateScene(a,orbit,1.0f/30);
    for(int i=0;i<144;++i)updateScene(b,orbit,1.0f/144);
    check(close(a.orbitYaw,b.orbitYaw),"Orbit speed depends on frame count");
    check(a.camera==CameraMode::Presentation,"Orbit input does not activate orbit view");
    Input mouse;mouse.mouse(100,-30);mouse.scroll(3);
    const auto delta=mouse.consume();
    check(delta.mouseDx==100&&delta.mouseDy==-30&&delta.scroll==3,"Mouse delta accumulation");
    check(mouse.consume().mouseDx==0&&mouse.consume().scroll==0,"Mouse deltas repeated");
    SceneState x,y;updateScene(x,delta,0);updateScene(y,delta,0.05f);
    check(x.orbitYaw==y.orbitYaw,"Mouse displacement incorrectly multiplied by deltaTime");
    InputState extreme;extreme.mouseDy=-100000;extreme.scroll=100000;updateScene(x,extreme,0);
    check(x.orbitPitch<=1.4f&&x.orbitRadius>=3,"Orbit bounds failed");
    InputState toggle;toggle.pause=true;toggle.palette=true;toggle.help=true;updateScene(x,toggle,0);
    check(x.paused&&x.alternatePalette&&!x.showHelp,"Scene interactions failed");
    InputState reset;reset.reset=true;updateScene(x,reset,0);
    check(!x.paused&&!x.alternatePalette&&x.showHelp&&x.orbitRadius==14,"New controls do not reset");
}

}
int main() {
    try { mathTests(); stateTests(); inputTests(); cameraTests(); newControlTests();
        std::cout<<"PASS: "<<checks<<" math, timing, input, state, and camera checks\n"; return 0;
    } catch(const std::exception& error) { std::cerr<<"FAIL: "<<error.what()<<'\n'; return 1; }
}
