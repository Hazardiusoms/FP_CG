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
    float orbitHorizontal = 0, orbitVertical = 0, zoom = 0;
    float mouseDx = 0, mouseDy = 0, scroll = 0;
    bool pause = false, palette = false, help = false;
    bool reset = false;
    bool changeCamera = false;
    bool toggleBloom = false;
    bool toggleCrt = false;
};
struct SceneState {
    double elapsedTime = 0;
    float orbitYaw = 0.5f, orbitPitch = 0.4f, orbitRadius = 14.0f;
    bool paused = false, alternatePalette = false, showHelp = true;
    float speed = initialSpeed;
    float carX = 0;               // Car module will be the sole writer.
    float followX = 0;           // Smoothed camera tracking, owned by the framework.
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
    if (input.pause) scene.paused = !scene.paused;
    if (input.palette) scene.alternatePalette = !scene.alternatePalette;
    if (input.help) scene.showHelp = !scene.showHelp;
    if (input.orbitHorizontal || input.orbitVertical || input.zoom || input.mouseDx || input.mouseDy || input.scroll)
        scene.camera = CameraMode::Presentation;
    scene.orbitYaw = std::remainder(scene.orbitYaw + input.orbitHorizontal*dt + input.mouseDx*0.005f, 6.2831853f);
    scene.orbitPitch = std::clamp(scene.orbitPitch + input.orbitVertical*dt - input.mouseDy*0.005f, 0.08f, 1.4f);
    scene.orbitRadius = std::clamp(scene.orbitRadius + input.zoom*dt*8.0f - input.scroll, 3.0f, 45.0f);
    scene.speed=std::clamp(scene.speed+input.speedAdjustment*speedAcceleration*dt,0.0f,maximumSpeed);
    if (input.changeCamera)
        scene.camera=scene.camera==CameraMode::Follow ? CameraMode::Presentation : CameraMode::Follow;
    // Effect requests are intentionally not enabled before those modules exist.
}
// Exponential tracking gives the same response at different frame rates.
inline void updateFollowCamera(SceneState& scene, float dt) {
    scene.followX += (scene.carX-scene.followX)*(1.0f-std::exp(-6.0f*std::max(dt,0.0f)));
}
inline double advanceGridOffset
(double offset, float speed, float dt) {
    const double result=std::fmod(offset+static_cast<double>(speed)*dt/gridWorldSize,1.0);
    return result<0 ? result+1.0 : result;
}
} // namespace sw
