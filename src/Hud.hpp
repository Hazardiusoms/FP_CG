#pragma once
#include "Shader.hpp"
#include "RenderContext.hpp"
#include "SceneState.hpp"
namespace sw {
class Hud {
public:
    Hud()=default;
    ~Hud(){shutdown();}
    Hud(const Hud&)=delete;
    Hud& operator=(const Hud&)=delete;
    void initialize(const std::filesystem::path& shaders);
    void render(const RenderContext& context,const SceneState& state,double fps);
    void shutdown();
private:
    Shader shader_;
    GLuint vao_=0,vbo_=0;
};
}
