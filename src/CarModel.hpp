#pragma once
#include "Math.hpp"
#include "SceneState.hpp"
#include <vector>
#include <cstdint>

namespace sw {
struct CarGeometry { std::vector<float> vertices; std::vector<std::uint32_t> indices; };
struct CarMaterial { Vec3 color; Vec3 emissive; };
struct CarPart { const char* name; int mesh; Mat4 model; CarMaterial material; };
// CPU model uses the framework math and state; no OpenGL context is required.
class CarModel {
public:
    static constexpr float wheelRadius=0.30f;
    static constexpr float lateralLimit=2.90f; // road 4 - conservative envelope 1 - margin .1
    void update(float dt,const InputState& input,SceneState& state);
    void reset(SceneState& state) { state.carX=0; state.steeringTilt=0; wheelAngle_=0; }
    float wheelAngle() const { return wheelAngle_; }
    std::vector<CarPart> parts(const SceneState& state) const;
    static CarGeometry box();
    static CarGeometry wheel();
    static CarGeometry wedge(bool cabin);
private:
    float wheelAngle_=0;
};
}
