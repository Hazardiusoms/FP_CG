#pragma once
#include "CarModel.hpp"
#include "LightingModel.hpp"
#include "Mesh.hpp"
#include "RenderContext.hpp"
#include "Shader.hpp"

namespace sw {
// Common lighting service: two lights, Blinn-Phong in world space. Owns the lit shader and the
// light time. Typical frame: update() once, then beginLit(), bindMaterial() per object, and
// drawLightMarker() last.
class Lighting {
public:
    void initialize(const std::filesystem::path& shaders);
    void update(float dt, const SceneState& state) { lightTime_ = advanceLightTime(lightTime_, dt, state.paused); }
    void reset() { lightTime_ = 0; }
    // Binds the lit program and uploads the per-frame uniforms (camera, ambient, both lights).
    void beginLit(const RenderContext& context, const SceneState& state) const;
    // Uploads model, normal matrix and material for the next draw call of the lit program.
    void bindMaterial(const Mat4& model, const Mat3& normal, const CarMaterial& material) const;
    // Small emissive cube showing where the moving light is; hidden while the light is off.
    void drawLightMarker(const SceneState& state) const;
    float lightTime() const { return lightTime_; }
    void shutdown();
private:
    Shader lit_;
    Mesh marker_;
    float lightTime_ = 0;
};
} // namespace sw
