#include "CarModel.hpp"
#include "Input.hpp"
#include "LightingModel.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace sw;
namespace {
int checks = 0;
void check(bool ok, const char* message) { ++checks; if (!ok) throw std::runtime_error(message); }
bool close(float a, float b, float tolerance = 1e-4f) { return std::abs(a - b) < tolerance; }
float lengthOf(Vec3 v) { return length(v); }

void lightMotionTests() {
    const Vec3 start = movingLightPosition(0, 0), later = movingLightPosition(2.0f, 0);
    check(lengthOf(later - start) > 1.0f, "Moving light does not move");
    // The light must stay above the car roof and clear of the car body at every time and carX.
    for (float carX : {-2.9f, 0.0f, 2.9f})
        for (int i = 0; i < 400; ++i) {
            const float t = lightLoopSeconds * i / 400.0f;
            const Vec3 p = movingLightPosition(t, carX);
            check(p.y > 1.5f, "Moving light dips into the car roof height");
            check(close(std::hypot(p.x - carX, p.z), lightOrbitRadius, 1e-3f), "Orbit radius drifts");
        }
    // The motion must repeat seamlessly where advanceLightTime wraps the clock.
    const Vec3 a = movingLightPosition(lightLoopSeconds - 1e-3f, 1.0f), b = movingLightPosition(0, 1.0f);
    check(lengthOf(a - b) < 0.02f, "Light jumps when the loop wraps");
}
void lightTimeTests() {
    check(advanceLightTime(1.0f, 0.1f, true) == 1.0f, "Paused light time advances");
    check(advanceLightTime(1.0f, -1.0f, false) == 1.0f && advanceLightTime(1.0f, NAN, false) == 1.0f,
          "Invalid time step accepted");
    check(close(advanceLightTime(1.0f, 100.0f, false), 1.05f), "Large time step not limited");
    float t = 0;
    for (int i = 0; i < 100000; ++i) t = advanceLightTime(t, 0.016f, false);
    check(t >= 0 && t < lightLoopSeconds, "Light time escapes its loop");
}
void frameTests() {
    SceneState state;
    const LightingFrame frame = computeLightingFrame(state, 1.0f);
    check(!frame.lights[0].point && frame.lights[1].point, "Expected one directional and one point light");
    check(close(length(frame.lights[0].vector), 1.0f), "Directional light vector is not unit length");
    check(frame.lights[0].color.x > 0 && frame.lights[1].color.z > 0, "A light is black by default");
    check(close(frame.lights[1].vector.x, movingLightPosition(1.0f, 0).x), "Point light ignores light time");
    // The point light follows the car sideways.
    state.carX = 2.0f;
    check(close(computeLightingFrame(state, 1.0f).lights[1].vector.x - frame.lights[1].vector.x, 2.0f),
          "Point light does not follow the car");
    state.carX = 0;
    state.movingLightEnabled = false;
    const LightingFrame off = computeLightingFrame(state, 1.0f);
    check(off.lights[1].color.x == 0 && off.lights[1].color.y == 0 && off.lights[1].color.z == 0,
          "Disabled moving light still emits");
    check(off.lights[0].color.x > 0, "Disabling the moving light also removes the sun");
    state.movingLightEnabled = true;
    state.alternatePalette = true;
    const LightingFrame alt = computeLightingFrame(state, 1.0f);
    check(alt.lights[1].color.x != frame.lights[1].color.x, "Palette switch does not recolour the light");
}
void toggleTests() {
    Input input; SceneState state; InputState idle;
    input.set(Key::F, true);
    updateScene(state, input.consume(), 0.01f);
    check(!state.movingLightEnabled, "F does not switch the moving light off");
    input.set(Key::F, true);                   // key repeat while held must not toggle again
    updateScene(state, input.consume(), 0.01f);
    check(!state.movingLightEnabled, "Held F retriggers the toggle");
    input.set(Key::F, false); input.set(Key::F, true);
    updateScene(state, input.consume(), 0.01f);
    check(state.movingLightEnabled, "Second F press does not switch the light back on");
    state.movingLightEnabled = false;
    InputState reset; reset.reset = true;
    updateScene(state, reset, 0);
    check(state.movingLightEnabled, "Reset does not restore the moving light");
}
// Normals must stay perpendicular to the surface for every real car part, including the
// non-uniformly scaled boxes and wheels, otherwise lighting is wrong on those faces.
void normalTests() {
    CarModel model; SceneState state; state.carX = 1.3f; state.steeringTilt = 0.08f;
    const CarGeometry geometry[] = {CarModel::box(), CarModel::wheel(), CarModel::wedge(false), CarModel::wedge(true)};
    int triangles = 0;
    for (const CarPart& part : model.parts(state)) {
        const CarGeometry& mesh = geometry[part.mesh];
        const Mat3 nm = normalMatrix(part.model);
        for (std::size_t t = 0; t + 2 < mesh.indices.size(); t += 3) {
            Vec3 world[3], n[3];
            for (int k = 0; k < 3; ++k) {
                const float* v = &mesh.vertices[mesh.indices[t + k] * 6];
                const Vec4 w = part.model * Vec4{v[0], v[1], v[2], 1};
                world[k] = {w.x, w.y, w.z};
                n[k] = nm * Vec3{v[3], v[4], v[5]};
            }
            const Vec3 e1 = world[1] - world[0], e2 = world[2] - world[0];
            const float scale = lengthOf(e1) * lengthOf(e2);
            if (!(scale > 1e-9f)) continue;
            check(std::abs(dot(n[0], e1)) < 1e-4f * lengthOf(n[0]) * lengthOf(e1) &&
                  std::abs(dot(n[0], e2)) < 1e-4f * lengthOf(n[0]) * lengthOf(e2), "Normal not perpendicular to a car face");
            ++triangles;
        }
    }
    check(triangles > 500, "Normal test covered too few triangles");
}
void materialTests() {
    CarModel model; SceneState state;
    float paint = -1, rubber = -1;
    for (const CarPart& part : model.parts(state)) {
        const CarMaterial& m = part.material;
        check(m.specular >= 0 && m.specular <= 1 && m.shininess >= 1, "Material specular parameters out of range");
        if (std::string(part.name) == "wedge body") paint = m.specular;
        if (std::string(part.name) == "wheel") rubber = m.specular;
    }
    check(paint > rubber && rubber >= 0, "Paint should be glossier than tyre rubber");
}
} // namespace
int main() {
    try {
        lightMotionTests(); lightTimeTests(); frameTests(); toggleTests(); normalTests(); materialTests();
    } catch (const std::exception& error) {
        std::cerr << "Lighting check failed: " << error.what() << '\n'; return 1;
    }
    std::cout << "PASS: " << checks << " lighting checks\n";
    return 0;
}
