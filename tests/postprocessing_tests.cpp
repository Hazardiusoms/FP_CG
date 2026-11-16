#include "Input.hpp"
#include "PostProcessingModel.hpp"
#include "SceneState.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace sw;
namespace {
int checks = 0;
void check(bool ok, const char* message) { ++checks; if (!ok) throw std::runtime_error(message); }

void weightTests() {
    for (float sigma : {0.5f, 1.0f, 2.0f, 4.0f, 0.0f, -3.0f}) {
        const auto w = gaussianWeights(sigma);
        float total = w[0];
        for (int i = 1; i < blurTapCount; ++i) {
            total += 2.0f * w[i];
            check(w[i] <= w[i - 1] && w[i] >= 0, "Gaussian weights must fall off from the centre");
            if (sigma >= 0.5f) check(w[i] > 0, "Side taps must contribute for a normal sigma");
        }
        check(std::abs(total - 1.0f) < 1e-5f, "Blur weights do not sum to 1 (energy would change)");
    }
    // A wider sigma spreads the energy: less in the centre.
    check(gaussianWeights(4.0f)[0] < gaussianWeights(1.0f)[0], "Wider sigma should lower the centre weight");
}
void sizeTests() {
    check(halfSize(1280) == 640 && halfSize(721) == 360, "Half size wrong");
    check(halfSize(1) == 1 && halfSize(0) == 1, "Half size must never reach zero");
    check(scanlinePeriod(720) == 3 && scanlinePeriod(1080) >= 4, "Unexpected scanline spacing");
    check(scanlinePeriod(1) == 2 && scanlinePeriod(0) == 2, "Scanlines must never be finer than two pixels");
    int previous = 0;
    for (int h = 100; h <= 4320; h += 37) {
        const int p = scanlinePeriod(h);
        check(p >= previous, "Scanline spacing must not shrink as the window grows");
        previous = p;
    }
    check(scanlineCompensation(0) == 1.0f, "No scanlines must not change brightness");
    check(scanlineCompensation(0.35f) > 1.0f && scanlineCompensation(5.0f) < 20.0f, "Scanline compensation out of range");
}
void defaultsTests() {
    const BloomSettings bloom; const CrtSettings crt;
    check(bloom.threshold > 0 && bloom.threshold < 1.5f && bloom.knee > 0, "Bloom threshold/knee out of range");
    check(bloom.intensity > 0 && bloom.intensity <= 1.5f && bloom.blurIterations >= 1, "Bloom strength out of range");
    check(crt.scanlineDepth > 0 && crt.scanlineDepth <= 0.6f, "CRT lines must stay subtle enough to read");
    check(crt.vignette >= 0 && crt.vignette <= 1.0f, "Vignette out of range");
}
void toggleTests() {
    Input input; SceneState state;
    check(!state.bloomEnabled && !state.crtEnabled, "Effects must be off by default");
    input.set(Key::B, true);
    updateScene(state, input.consume(), 0.01f);
    check(state.bloomEnabled && !state.crtEnabled, "B should enable only bloom");
    input.set(Key::B, true);   // held key must not retrigger
    updateScene(state, input.consume(), 0.01f);
    check(state.bloomEnabled, "Held B retriggers the toggle");
    input.set(Key::B, false);
    input.set(Key::T, true);
    updateScene(state, input.consume(), 0.01f);
    check(state.bloomEnabled && state.crtEnabled, "T should enable CRT independently of bloom");
    input.set(Key::T, false); input.set(Key::B, true);
    updateScene(state, input.consume(), 0.01f);
    check(!state.bloomEnabled && state.crtEnabled, "Second B press should disable bloom only");
    InputState reset; reset.reset = true;
    updateScene(state, reset, 0);
    check(!state.bloomEnabled && !state.crtEnabled, "Reset should switch effects off");
}
} // namespace
int main() {
    try { weightTests(); sizeTests(); defaultsTests(); toggleTests(); }
    catch (const std::exception& error) { std::cerr << "Post-processing check failed: " << error.what() << '\n'; return 1; }
    std::cout << "PASS: " << checks << " post-processing checks\n";
    return 0;
}
