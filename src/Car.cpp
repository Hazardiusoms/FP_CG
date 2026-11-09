#include "Car.hpp"

namespace sw {
void Car::initialize(const std::filesystem::path& shaders) {
    shutdown();
    const auto box=CarModel::box(),wheel=CarModel::wheel();
    box_.initialize(box.vertices,6,box.indices,{{0,3,0},{1,3,3}});
    wheel_.initialize(wheel.vertices,6,wheel.indices,{{0,3,0},{1,3,3}});
    const auto wedge=CarModel::wedge(false),cabin=CarModel::wedge(true);
    wedge_.initialize(wedge.vertices,6,wedge.indices,{{0,3,0},{1,3,3}});
    cabin_.initialize(cabin.vertices,6,cabin.indices,{{0,3,0},{1,3,3}});
    preview_.initialize(shaders/"car_preview.vert",shaders/"car_preview.frag");
}
void Car::drawParts(const SceneState& state,const BindMaterial& bind) const {
    for(const auto& part:model_.parts(state)) {
        bind(part.model,normalMatrix(part.model),part.material);
        (part.mesh==0 ? box_ : part.mesh==1 ? wheel_ : part.mesh==2 ? wedge_ : cabin_).draw();
    }
}
void Car::render(const RenderContext& context,const SceneState& state) const {
    preview_.use(); preview_.set("uView",context.view); preview_.set("uProjection",context.projection);
    drawParts(state,[&](const Mat4& model,const Mat3& normal,const CarMaterial& material) {
        preview_.set("uModel",model); preview_.set("uNormalMatrix",normal);
        preview_.set("uBaseColor",material.color); preview_.set("uEmissive",material.emissive);
    });
}
void Car::shutdown() { box_.shutdown(); wheel_.shutdown(); wedge_.shutdown(); cabin_.shutdown(); preview_.shutdown(); }
}
