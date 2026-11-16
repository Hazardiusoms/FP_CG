#include "Lighting.hpp"
#include <cstdio>

namespace sw {
void Lighting::initialize(const std::filesystem::path& shaders) {
    shutdown();
    lit_.initialize(shaders / "lit.vert", shaders / "lit.frag");
    const auto cube = CarModel::box();   // position + normal, same layout the lit shader expects
    marker_.initialize(cube.vertices, 6, cube.indices, {{0, 3, 0}, {1, 3, 3}});
}

void Lighting::beginLit(const RenderContext& context, const SceneState& state, float lampTravel) const {
    const LightingFrame frame = computeLightingFrame(state, lightTime_, lampTravel);
    lit_.use();
    lit_.set("uView", context.view);
    lit_.set("uProjection", context.projection);
    lit_.set("uCameraPosition", context.cameraPosition);
    lit_.set("uAmbient", frame.ambient);
    for (int i = 0; i < lightCount; ++i) {
        const LightSource& light = frame.lights[i];
        char name[40];
        std::snprintf(name, sizeof name, "uLights[%d].vector", i);
        lit_.set(name, light.vector);
        std::snprintf(name, sizeof name, "uLights[%d].isPoint", i);
        lit_.set(name, light.point ? 1.0f : 0.0f);
        std::snprintf(name, sizeof name, "uLights[%d].color", i);
        lit_.set(name, light.color);
        std::snprintf(name, sizeof name, "uLights[%d].linear", i);
        lit_.set(name, light.linearAttenuation);
        std::snprintf(name, sizeof name, "uLights[%d].quadratic", i);
        lit_.set(name, light.quadraticAttenuation);
    }
}

void Lighting::bindMaterial(const Mat4& model, const Mat3& normal, const CarMaterial& material) const {
    lit_.set("uModel", model);
    lit_.set("uNormalMatrix", normal);
    lit_.set("uBaseColor", material.color);
    lit_.set("uEmissive", material.emissive);
    lit_.set("uSpecular", material.specular);
    lit_.set("uShininess", material.shininess);
}

void Lighting::drawLightMarker(const SceneState& state) const {
    if (!state.movingLightEnabled) return;
    const LightingFrame frame = computeLightingFrame(state, lightTime_);
    const Mat4 model = translation(frame.lights[1].vector) * scaling({0.1f, 0.1f, 0.1f});
    // Pure emissive: black base colour, no highlight, so only the light colour shows.
    bindMaterial(model, normalMatrix(model), CarMaterial{{0, 0, 0}, frame.lights[1].color * 0.6f, 0.0f, 1.0f});
    marker_.draw();
}

void Lighting::shutdown() {
    marker_.shutdown();
    lit_.shutdown();
}
} // namespace sw
