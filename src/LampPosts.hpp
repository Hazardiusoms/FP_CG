#pragma once
// Roadside lamp posts: the numbers shared by the road meshes and by the lighting, so the glowing
// lamp heads, the light pools on the road and the lights that fall on the car all agree.
// CPU-only (no OpenGL), so it can be unit-tested.
#include "Math.hpp"
#include <array>
#include <algorithm>
#include <cmath>

namespace sw {
constexpr float lampSpacing = 12.0f;   // metres between posts along the road
constexpr float lampPoleX = 5.15f;     // post distance from the road centre line
constexpr float lampHeadX = 4.0f;      // end of the arm: centre of the lamp head
constexpr float lampHeadY = 2.94f;     // just under the head plate
constexpr int lampsPerSide = 4;        // posts that can light the car at once, per side
constexpr int lampLightCount = 2 * lampsPerSide;
constexpr float lampFadeStart = 14.0f; // beyond this distance from the car a lamp light fades out
constexpr float lampFadeEnd = 22.0f;   // ...and is fully off here, so posts never pop in or out

// Lamp colour: warm pink in the default palette, cool blue in the alternate one (G).
inline Vec3 lampColor(bool alternatePalette) {
    return alternatePalette ? Vec3{0.45f, 0.80f, 1.0f} : Vec3{1.0f, 0.45f, 0.72f};
}

struct LampLight {
    Vec3 position;    // world position of the lamp head
    float strength;   // 0..1, fades to 0 with distance from the car
};

// Posts sit at 12 m steps that slide along the road with the road's travel (0..12 m, the same
// value the road shaders use). Returns the lamps around the car in a fixed order.
inline std::array<LampLight, lampLightCount> lampLights(float travel, float carZ = 0.0f) {
    float phase = std::fmod(travel, lampSpacing);
    if (phase < 0) phase += lampSpacing;
    std::array<LampLight, lampLightCount> lamps{};
    int index = 0;
    for (int n = -lampsPerSide / 2; n < lampsPerSide - lampsPerSide / 2; ++n)
        for (float side : {-1.0f, 1.0f}) {
            const float z = lampSpacing * static_cast<float>(n) + phase;
            const float t = std::clamp((std::abs(z - carZ) - lampFadeStart) / (lampFadeEnd - lampFadeStart), 0.0f, 1.0f);
            lamps[static_cast<std::size_t>(index++)] = {{side * lampHeadX, lampHeadY, z}, 1.0f - t * t * (3.0f - 2.0f * t)};
        }
    return lamps;
}
} // namespace sw
