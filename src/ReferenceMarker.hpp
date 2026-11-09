#pragma once
#include "Shader.hpp"
#include "RenderContext.hpp"

namespace sw {
// Temporary stationary size/depth reference. This is NOT Turar's car module.
class ReferenceMarker {
public:
    ReferenceMarker()=default;
    ~ReferenceMarker() { shutdown(); }
    ReferenceMarker(const ReferenceMarker&)=delete;
    ReferenceMarker& operator=(const ReferenceMarker&)=delete;
    void initialize(const std::filesystem::path& shaderDirectory);
    void render(const RenderContext& context) const;
    void shutdown();
private:
    Shader shader_;
    GLuint vao_=0,vbo_=0;
};
}
