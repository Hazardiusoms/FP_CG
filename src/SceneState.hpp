#pragma once
#include <algorithm>
#include <cmath>

namespace sw {
constexpr float roadHalfWidth = 4.0f;
constexpr float initialSpeed = 12.0f;
constexpr float maximumSpeed = 40.0f;
constexpr float speedAcceleration = 8.0f;
constexpr float gridWorldSize = 2.0f;
enum class CameraMode { Follow, Presentation };
struct InputState {
    float steering = 0;
    float speedAdjustment = 0;
    bool reset = false;
    bool changeCamera = false;
    bool toggleBloom = false;
    bool toggleCrt = false;
};
struct SceneState {
    double elapsedTime = 0;
    float speed = initialSpeed;
    float carX = 0;               // Car module will be the sole writer.
    float steeringTilt = 0;       // Radians; Car module will be the sole writer.
    CameraMode camera = CameraMode::Follow;
    bool bloomEnabled = false;   // Reserved; no effect implemented in this build.
    bool crtEnabled = false;
};
inline float frameStep(double elapsed) {
    if (!std::isfinite(elapsed)) return 0;
    return static_cast<float>(std::clamp(elapsed,0.0,0.05));
}
inline void updateScene(SceneState& scene, const InputState& input, float dt) {
    if (input.reset) { scene=SceneState{}; return; }
    scene.elapsedTime+=dt;
    scene.speed=std::clamp(scene.speed+input.speedAdjustment*speedAcceleration*dt,0.0f,maximumSpeed);
    if (input.changeCamera)
        scene.camera=scene.camera==CameraMode::Follow ? CameraMode::Presentation : CameraMode::Follow;
    // Effect requests are intentionally not enabled before those modules exist.
}
inline double advanceGridOffset(double offset, float speed, float dt) {
    const double result=std::fmod(offset+static_cast<double>(speed)*dt/gridWorldSize,1.0);
    return result<0 ? result+1.0 : result;
}
} // namespace sw
