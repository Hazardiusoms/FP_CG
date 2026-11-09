#pragma once
#include "CarModel.hpp"
#include "Mesh.hpp"
#include "RenderContext.hpp"
#include "Shader.hpp"
#include <functional>

namespace sw {
class Car {
public:
    void initialize(const std::filesystem::path& shaders);
    void update(float dt,const InputState& input,SceneState& state) { model_.update(dt,input,state); }
    void reset(SceneState& state) { model_.reset(state); }
    void render(const RenderContext& context,const SceneState& state) const;
    // Future Lighting service binds its program/frame/model/normal/material uniforms here.
    using BindMaterial=std::function<void(const Mat4&,const Mat3&,const CarMaterial&)>;
    void drawParts(const SceneState& state,const BindMaterial& bind) const;
    void shutdown();
private:
    CarModel model_;
    Mesh box_,wheel_,wedge_,cabin_;
    Shader preview_;
};
}
