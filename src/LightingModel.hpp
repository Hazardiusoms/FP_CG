#pragma once
// CPU-side description of the scene lights. No OpenGL here, so the numbers can be unit-tested.
// World space is used for everything: +Y up, +X right, the car faces -Z.
#include "Math.hpp"
#include "SceneState.hpp"
#include <algorithm>
#include <cmath>

namespace sw {
constexpr int lightCount = 2;

struct LightSource {
    // Directional light: unit vector pointing TOWARD the light. Point light: world position.
    Vec3 vector;
    bool point = false;
    Vec3 color;                       // linear RGB, already multiplied by intensity
    float linearAttenuation = 0;      // point lights only
    float quadraticAttenuation = 0;
};

struct LightingFrame {
    Vec3 ambient;
    LightSource lights[lightCount];   // [0] sun (directional), [1] moving accent (point)
};

// The moving light orbits the car at a fixed height band. Orbit and bob speeds are integer
// multiples of one base angular speed, so the motion repeats exactly every lightLoopSeconds.
constexpr float lightBaseSpeed = 0.3f;                        // rad/s
constexpr float lightLoopSeconds = 2.0f * pi / lightBaseSpeed;
constexpr float lightOrbitRadius = 3.6f;
constexpr float lightHeight = 2.4f, lightBob = 0.5f;          // always above the 1.1 m roof

inline Vec3 movingLightPosition(float lightTime, float carX) {
    const float orbit = 3.0f * lightBaseSpeed * lightTime;
    const float bob = 2.0f * lightBaseSpeed * lightTime;
    return {carX + lightOrbitRadius * std::sin(orbit), lightHeight + lightBob * std::sin(bob),
            lightOrbitRadius * std::cos(orbit)};
}

// Light time is owned by the Lighting module. It stops with the road when paused and wraps by
// whole loops so float precision never degrades during a long session.
inline float advanceLightTime(float lightTime, float dt, bool paused) {
    if (paused || !std::isfinite(dt) || dt <= 0) return lightTime;
    return std::fmod(lightTime + std::min(dt, 0.05f), lightLoopSeconds);
}

inline LightingFrame computeLightingFrame(const SceneState& state, float lightTime) {
    const float alt = state.alternatePalette ? 1.0f : 0.0f;
    auto mix = [alt](Vec3 a, Vec3 b) { return a * (1.0f - alt) + b * alt; };
    LightingFrame frame;
    frame.ambient = mix({0.10f, 0.07f, 0.17f}, {0.07f, 0.09f, 0.16f});
    // The sun sits ahead of the car, slightly raised so roof and bonnet catch a highlight.
    frame.lights[0].vector = normalized({0.0f, 0.35f, -1.0f});
    frame.lights[0].color = mix({1.00f, 0.52f, 0.42f}, {0.50f, 0.72f, 1.00f}) * 0.95f;
    LightSource& accent = frame.lights[1];
    accent.point = true;
    accent.vector = movingLightPosition(lightTime, state.carX);
    accent.color = state.movingLightEnabled
        ? mix({0.15f, 0.85f, 1.00f}, {1.00f, 0.55f, 0.15f}) * 2.6f : Vec3{};
    accent.linearAttenuation = 0.08f;
    accent.quadraticAttenuation = 0.035f;
    return frame;
}
} // namespace sw
