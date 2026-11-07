#pragma once
#include "Shader.hpp"
#include "RenderContext.hpp"
#include "SceneState.hpp"

namespace sw {
class Road {
public:
    Road()=default;
    ~Road() { shutdown(); }
    Road(const Road&)=delete;
    Road& operator=(const Road&)=delete;
    void initialize(const std::filesystem::path& shaderDirectory);
    void update(float deltaTime,float speed);
    void render(const RenderContext& context) const;
    void reset() { offset_=0; }
    void shutdown();
    double offset() const { return offset_; }
private:
    Shader shader_;
    GLuint vao_=0,vbo_=0;
    double offset_=0;
};
} // namespace sw
