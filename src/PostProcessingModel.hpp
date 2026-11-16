#pragma once
// CPU-side parameters for the bloom and CRT passes. No OpenGL here, so they can be unit-tested
// and the same numbers are uploaded to the shaders (one source of truth).
#include <algorithm>
#include <array>
#include <cmath>

namespace sw {
struct BloomSettings {
    float threshold = 0.65f;   // peak channel above this starts to glow
    float knee = 0.30f;        // width of the soft transition below the threshold
    float intensity = 1.05f;   // how much blurred glow is added to the scene
    int blurIterations = 6;    // horizontal + vertical pass pairs at half resolution
    float sigma = 2.0f;        // Gaussian width in half-resolution texels
};
struct CrtSettings {
    float scanlineDepth = 0.35f;  // 0 = no lines, 1 = fully black gaps
    float vignette = 0.45f;       // corner darkening strength
};

constexpr int blurTapCount = 5;   // centre + 4 taps on each side
// One-sided, normalised Gaussian: weights[0] + 2 * (weights[1] + ... ) == 1, so blur keeps energy.
inline std::array<float, blurTapCount> gaussianWeights(float sigma) {
    std::array<float, blurTapCount> w{};
    const float s = std::max(sigma, 0.1f);
    float total = 0;
    for (int i = 0; i < blurTapCount; ++i) {
        w[i] = std::exp(-0.5f * float(i * i) / (s * s));
        total += i == 0 ? w[i] : 2.0f * w[i];
    }
    for (float& value : w) value /= total;
    return w;
}
// Scanline spacing in framebuffer pixels: about 240 lines across the picture, never finer than
// two pixels, always a whole number so lines do not crawl or alias with the pixel grid.
inline int scanlinePeriod(int framebufferHeight) {
    return std::max(2, static_cast<int>(std::lround(framebufferHeight / 240.0)));
}
// Bloom works at half resolution, never below one texel in either direction.
inline int halfSize(int size) { return std::max(1, size / 2); }
// Multiplier that keeps the average picture brightness when scanlines darken the gaps.
inline float scanlineCompensation(float depth) { return 1.0f / (1.0f - 0.5f * std::clamp(depth, 0.0f, 0.9f)); }
} // namespace sw
