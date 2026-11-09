#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.hpp"
#include "Input.hpp"
#include "Hud.hpp"
#include <iomanip>
#include "Road.hpp"
#include "ReferenceMarker.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace {
using namespace sw;
namespace fs=std::filesystem;
struct Options { fs::path assets, capture, smoke; bool help=false; int benchmarkFrames=0; };
Options parse(int argc,char** argv) {
    Options o;
    for(int i=1;i<argc;++i) {
        const std::string arg=argv[i];
        if(arg=="--help") { o.help=true; continue; }
        if(arg=="--benchmark") {
            if(i+1>=argc) throw std::runtime_error("Missing benchmark frame count");
            o.benchmarkFrames=std::stoi(argv[++i]);
            if(o.benchmarkFrames<120) throw std::runtime_error("Use at least 120 benchmark frames");
            continue;
        }
        if(arg!="--assets" && arg!="--capture" && arg!="--smoke-test")
            throw std::runtime_error("Unknown option: "+arg+" (use --help)");
        if(i+1>=argc) throw std::runtime_error("Missing argument for "+arg);
        const fs::path value=argv[++i];
        if(arg=="--assets") o.assets=value;
        else if(arg=="--capture") o.capture=value;
        else o.smoke=value;
    }
    if(int(!o.capture.empty()) + int(!o.smoke.empty()) + int(o.benchmarkFrames>0) > 1)
        throw std::runtime_error("Choose one of --capture, --smoke-test or --benchmark");
    return o;
}
fs::path executableDirectory() {
#if defined(__APPLE__)
    uint32_t length=0; _NSGetExecutablePath(nullptr,&length);
    std::vector<char> buffer(length);
    if(_NSGetExecutablePath(buffer.data(),&length)!=0) throw std::runtime_error("Cannot locate executable");
    return fs::weakly_canonical(buffer.data()).parent_path();
#elif defined(_WIN32)
    std::vector<wchar_t> buffer(32768);
    const DWORD length=GetModuleFileNameW(nullptr,buffer.data(),static_cast<DWORD>(buffer.size()));
    if(!length || length==buffer.size()) throw std::runtime_error("Cannot locate executable");
    return fs::path(std::wstring(buffer.data(),length)).parent_path();
#elif defined(__linux__)
    return fs::read_symlink("/proc/self/exe").parent_path();
#else
    throw std::runtime_error("Executable lookup unsupported; provide --assets <shader-directory>");
#endif
}
std::optional<Key> translateKey(int key) {
    switch(key) {
        case GLFW_KEY_LEFT: return Key::Left; case GLFW_KEY_RIGHT: return Key::Right;
        case GLFW_KEY_A: return Key::A; case GLFW_KEY_D: return Key::D;
        case GLFW_KEY_W: return Key::W; case GLFW_KEY_S: return Key::S;
        case GLFW_KEY_C: return Key::C; case GLFW_KEY_R: return Key::R;
        case GLFW_KEY_B: return Key::B; case GLFW_KEY_T: return Key::T;
        case GLFW_KEY_I: return Key::I; case GLFW_KEY_J: return Key::J;
        case GLFW_KEY_K: return Key::K; case GLFW_KEY_L: return Key::L;
        case GLFW_KEY_U: return Key::U; case GLFW_KEY_O: return Key::O;
        case GLFW_KEY_SPACE: return Key::Space; case GLFW_KEY_G: return Key::G; case GLFW_KEY_H: return Key::H;
        case GLFW_KEY_ESCAPE: return Key::Escape;
        default: return std::nullopt;
    }
}
class Window {
public:
    GLFWwindow* handle=nullptr;
    Input input; double mouseX=0,mouseY=0; bool dragging=false;
    explicit Window(bool hidden) {
        glfwSetErrorCallback([](int code,const char* message) {
            std::cerr<<"GLFW error "<<code<<": "<<message<<'\n';
        });
        if(!glfwInit()) throw std::runtime_error("GLFW initialization failed");
#ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER,GL_FALSE);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
#endif
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DEPTH_BITS,24);
        glfwWindowHint(GLFW_SAMPLES,4);
        glfwWindowHint(GLFW_VISIBLE,hidden ? GLFW_FALSE : GLFW_TRUE);
        handle=glfwCreateWindow(1280,720,"Synthwave Drive | Almas framework",nullptr,nullptr);
        if(!handle) { glfwTerminate(); throw std::runtime_error("Cannot create an OpenGL window"); }
        glfwMakeContextCurrent(handle); glewExperimental=GL_TRUE;
        const GLenum status=glewInit();
        if(status!=GLEW_OK) {
            const std::string error=reinterpret_cast<const char*>(glewGetErrorString(status));
            glfwDestroyWindow(handle); glfwTerminate(); handle=nullptr;
            throw std::runtime_error("GLEW initialization failed: "+error);
        }
        // Some GLEW/core-context combinations probe a removed extension query.
        while(glGetError()!=GL_NO_ERROR) {}
        glfwSwapInterval(hidden ? 0 : 1);
        glfwSetWindowUserPointer(handle,this);
        glfwSetKeyCallback(handle,[](GLFWwindow* window,int key,int,int action,int) {
            if(const auto mapped=translateKey(key))
                static_cast<Window*>(glfwGetWindowUserPointer(window))->input.set(*mapped,action!=GLFW_RELEASE);
        });
        glfwSetWindowFocusCallback(handle,[](GLFWwindow* window,int focused) {
            if(!focused) { auto* self=static_cast<Window*>(glfwGetWindowUserPointer(window)); self->input.clear(); self->dragging=false; }
        });
        glfwSetMouseButtonCallback(handle,[](GLFWwindow* window,int button,int action,int){
            if(button!=GLFW_MOUSE_BUTTON_RIGHT)return;
            auto* self=static_cast<Window*>(glfwGetWindowUserPointer(window));
            self->dragging=action==GLFW_PRESS;
            glfwGetCursorPos(window,&self->mouseX,&self->mouseY);
        });
        glfwSetCursorPosCallback(handle,[](GLFWwindow* window,double x,double y){
            auto* self=static_cast<Window*>(glfwGetWindowUserPointer(window));
            if(self->dragging)self->input.mouse(float(x-self->mouseX),float(y-self->mouseY));
            self->mouseX=x;self->mouseY=y;
        });
        glfwSetScrollCallback(handle,[](GLFWwindow* window,double,double y){
            static_cast<Window*>(glfwGetWindowUserPointer(window))->input.scroll(float(y));
        });
        glfwSetFramebufferSizeCallback(handle,[](GLFWwindow*,int width,int height) {
            glViewport(0,0,width,height);
        });
        std::cout<<"OpenGL: "<<glGetString(GL_VERSION)<<"\nRenderer: "<<glGetString(GL_RENDERER)<<'\n';
    }
    ~Window() { if(handle) glfwDestroyWindow(handle); glfwTerminate(); }
    Window(const Window&)=delete; Window& operator=(const Window&)=delete;
};
void checkGl(const char* stage) {
    const GLenum error=glGetError();
    if(error!=GL_NO_ERROR) {
        std::ostringstream message; message<<"OpenGL error at "<<stage<<": 0x"<<std::hex<<error;
        throw std::runtime_error(message.str());
    }
}
struct Frame { int width=0,height=0; std::vector<unsigned char> rgb; };
Frame readFrame(const RenderContext& ctx) {
    Frame frame{ctx.framebufferWidth,ctx.framebufferHeight,{}};
    frame.rgb.resize(static_cast<std::size_t>(frame.width)*frame.height*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1); glReadBuffer(GL_BACK);
    glReadPixels(0,0,frame.width,frame.height,GL_RGB,GL_UNSIGNED_BYTE,frame.rgb.data());
    glPixelStorei(GL_PACK_ALIGNMENT,4); checkGl("readback");
    return frame;
}
void writeFrame(const fs::path& path,const Frame& frame) {
    if(!path.parent_path().empty()) fs::create_directories(path.parent_path());
    std::ofstream output(path,std::ios::binary);
    if(!output) throw std::runtime_error("Cannot write screenshot: "+path.string());
    output<<"P6\n"<<frame.width<<' '<<frame.height<<"\n255\n";
    for(int y=frame.height-1;y>=0;--y)
        output.write(reinterpret_cast<const char*>(frame.rgb.data()+static_cast<std::size_t>(y)*frame.width*3),frame.width*3);
    if(!output) throw std::runtime_error("Screenshot write failed: "+path.string());
}
void require(bool ok,const char* message) { if(!ok) throw std::runtime_error(message); }
void requireVisible(const Frame& frame) {
    std::size_t bright=0;
    for(std::size_t i=0;i<frame.rgb.size();i+=3)
        if(frame.rgb[i]>40 || frame.rgb[i+1]>40 || frame.rgb[i+2]>40) ++bright;
    require(bright>static_cast<std::size_t>(frame.width)*frame.height/100,"Render check: scene is blank or almost blank");
}

int run(const Options& options) {
    const bool hidden=!options.capture.empty() || !options.smoke.empty();
    Window window(hidden);
    // Declared after Window: all GPU resources are destroyed before the context.
    Hud hud; double displayedFps=0; Road road; ReferenceMarker reference; Camera camera; SceneState state; RenderContext context;
    const fs::path shaders=options.assets.empty() ? executableDirectory()/"shaders" : options.assets;
    road.initialize(shaders); reference.initialize(shaders); hud.initialize(shaders); checkGl("initialization");
    glfwPollEvents();
    std::cout<<"Shaders: "<<shaders<<"\nW/S speed | C camera | R reset | Esc exit\n"
             <<"Stationary block = temporary size reference. Car/environment/lighting are pending.\n";

    auto update=[&](const InputState& input,float dt) {
        updateScene(state,input,dt);
        if(input.reset) { road.reset(); return; }
        // Turar: car.update(dt,input,state) goes here before the camera update.
        updateFollowCamera(state,dt);
        road.update(dt,state.paused ? 0.0f : state.speed);
        // Muslim: environment.update(dt,state.speed) goes here.
    };
    auto draw=[&]() -> bool {
        int width=0,height=0; glfwGetFramebufferSize(window.handle,&width,&height);
        if(!camera.update(state,width,height,context)) return false;
        glViewport(0,0,width,height);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glDepthMask(GL_TRUE); glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
        glDisable(GL_BLEND); glDisable(GL_CULL_FACE);
        glClearColor(0.018f,0.012f,0.040f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        // Muslim: background pass, then restore depth test and depth writes.
        road.render(context,state.alternatePalette);
        // Muslim: mountains. Nursalim/Turar: lit car replaces this reference.
        reference.render(context);
        hud.render(context,state,displayedFps);
        checkGl("frame render"); return true;
    };
    if(!options.capture.empty()) {
        require(draw(),"Cannot capture a zero-sized framebuffer");
        const Frame frame=readFrame(context); requireVisible(frame); writeFrame(options.capture,frame);
        std::cout<<"Captured "<<options.capture<<'\n'; return 0;
    }
    if(!options.smoke.empty()) {
        fs::create_directories(options.smoke);
        auto capture=[&](const char* name) {
            require(draw(),"Smoke test: zero-sized framebuffer");
            Frame frame=readFrame(context); requireVisible(frame);
            writeFrame(options.smoke/name,frame); return frame;
        };
        const Frame initial=capture("follow.ppm");
        road.update(0.02f,state.speed);
        require(capture("moving.ppm").rgb!=initial.rgb,"Grid does not move");
        road.reset(); road.update(1.0f,12.0f);
        require(capture("wrapped.ppm").rgb==initial.rgb,"Whole UV period does not repeat exactly");
        road.update(10.0f,0.0f);
        require(capture("stopped.ppm").rgb==initial.rgb,"Zero speed moves road furniture or markings");
        road.update(12000.0f,12.0f);
        require(capture("long-run.ppm").rgb==initial.rgb,"Long-run road cycle drifts");
        window.input.set(Key::C,true); update(window.input.consume(),0);

        require(state.camera==CameraMode::Presentation,"Camera key did not switch view");
        const Frame presentation=capture("presentation.ppm");
        require(presentation.rgb!=initial.rgb,"Camera views are identical");
        window.input.set(Key::C,true); update(window.input.consume(),0);
        require(state.camera==CameraMode::Presentation,"Held camera key toggled again");
        window.input.set(Key::C,false);
        window.input.set(Key::W,true); update(window.input.consume(),0.05f);
        require(state.speed>initialSpeed,"Speed control failed");
        window.input.set(Key::W,false);
        window.input.set(Key::R,true); update(window.input.consume(),0);
        require(state.speed==initialSpeed && state.camera==CameraMode::Follow && road.offset()==0,"Reset failed");
        require(capture("reset.ppm").rgb==initial.rgb,"Reset does not restore initial image");
        window.input.clear();
        glfwSetWindowSize(window.handle,640,900); glfwPollEvents();
        const Frame portrait=capture("portrait.ppm");
        require(portrait.width<portrait.height,"Portrait resize failed");
        window.input.mouse(90,-20); window.input.scroll(2); update(window.input.consume(),0);
        const Frame orbited=capture("mouse-orbit.ppm");
        require(orbited.rgb!=portrait.rgb,"Mouse orbit did not change render");
        const double beforePause=road.offset();
        InputState pause;pause.pause=true;update(pause,0.02f);
        require(road.offset()==beforePause,"Paused road moved");
        InputState palette;palette.palette=true;update(palette,0);
        require(capture("palette.ppm").rgb!=orbited.rgb,"Palette did not change render");
        glfwSetWindowSize(window.handle,320,240); glfwPollEvents();
        capture("small.ppm");
        const auto savedProjection=context.projection;

        require(!camera.update(state,0,0,context),"Zero-sized framebuffer accepted");
        require(context.projection.at(0,0)==savedProjection.at(0,0),"Zero size changed projection");
        checkGl("smoke completion");
        std::cout<<"PASS: visible scene, grid motion, UV repeat, camera edge input, speed, reset, resize, mouse orbit, pause, palette, zero-size guard, GL errors.\n";
        return 0;
    }
    if(options.benchmarkFrames)glfwSwapInterval(0);
    double previous=glfwGetTime(),nextTitle=0,fpsStart=previous;
    int fpsFrames=0,benchmarkFrame=0;
    std::vector<double> frameTimes;
    while(!glfwWindowShouldClose(window.handle)) {
        const double frameStart=glfwGetTime();
        glfwPollEvents();
        if(window.input.down(Key::Escape)) glfwSetWindowShouldClose(window.handle,GLFW_TRUE);
        const double now=glfwGetTime(); const double actualDt=now-previous;
        const float dt=frameStep(actualDt); previous=now;
        ++fpsFrames;
        if(now-fpsStart>=0.5){displayedFps=fpsFrames/(now-fpsStart);fpsFrames=0;fpsStart=now;}
        int width=0,height=0; glfwGetFramebufferSize(window.handle,&width,&height);
        if(width<=0 || height<=0) { glfwWaitEventsTimeout(0.05); previous=glfwGetTime(); continue; }
        update(window.input.consume(),dt);
        if(draw()) glfwSwapBuffers(window.handle);
        if(options.benchmarkFrames){
            glFinish();
            if(benchmarkFrame++>=60)frameTimes.push_back(glfwGetTime()-frameStart);
            if(benchmarkFrame>=options.benchmarkFrames+60){
                double total=0,maxTime=0;
                for(double t:frameTimes){total+=t;maxTime=std::max(maxTime,t);}
                auto sorted=frameTimes;std::sort(sorted.begin(),sorted.end());
                const double p95=sorted[static_cast<std::size_t>((sorted.size()-1)*0.95)];
                std::cout<<"BENCHMARK width="<<width<<" height="<<height<<" samples="<<frameTimes.size()
                         <<" avg_fps="<<frameTimes.size()/total<<" p95_ms="<<p95*1000
                         <<" worst_ms="<<maxTime*1000<<" vsync=off glFinish=on\n";
                return 0;
            }
        }
        if(now>=nextTitle) {
            std::ostringstream title;
            title<<"Synthwave Drive | FPS "<<std::fixed<<std::setprecision(1)<<displayedFps<<" | W/S: speed "<<static_cast<int>(state.speed)
                 <<" | C: "<<(state.camera==CameraMode::Follow ? "Follow" : "Presentation")
                 <<" | R: reset | Esc: exit | Temporary block";
            glfwSetWindowTitle(window.handle,title.str().c_str()); nextTitle=now+0.2;
        }
    }
    return 0;
}
}
int main(int argc,char** argv) {
    try {
        const Options options=parse(argc,argv);
        if(options.help) {
            std::cout<<"Synthwave Drive — Almas Week 8 framework\n"
                     <<"Run without arguments for the interactive window.\n"
                     <<"--benchmark N      Measure N frames after 60 warm-up frames, then exit\n"
                     <<"--assets DIR       Override the shader directory\n"
                     <<"--capture FILE.ppm Render one frame and exit\n"
                     <<"--smoke-test DIR   Run real OpenGL checks and save frames\n";
            return 0;
        }
        return run(options);
    } catch(const std::exception& error) {
        std::cerr<<"Synthwave Drive: "<<error.what()<<'\n'; return EXIT_FAILURE;
    }
}
