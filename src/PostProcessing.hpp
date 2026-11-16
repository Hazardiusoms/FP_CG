#pragma once
#include "PostProcessingModel.hpp"
#include "SceneState.hpp"
#include "Shader.hpp"
#include <GL/glew.h>
#include <filesystem>

namespace sw {
// Optional screen-space effects: bloom (B) and CRT scanlines (T).
//
// Frame flow, driven by main.cpp:
//   begin()  - if an effect is on, binds the off-screen multisampled scene target, otherwise the
//              window framebuffer (so with both effects off the frame is rendered exactly as before);
//   ...draw the scene...
//   finish() - resolves MSAA, runs bright-pass + separable blur when bloom is on, then composites
//              scene + bloom (+ CRT) to the window framebuffer. The HUD is drawn afterwards, so
//              text stays sharp and is never blurred or striped.
// If the off-screen targets cannot be created, the effects switch themselves off and the core
// scene keeps rendering normally.
class PostProcessing {
public:
    PostProcessing() = default;
    ~PostProcessing() { shutdown(); }
    PostProcessing(const PostProcessing&) = delete;
    PostProcessing& operator=(const PostProcessing&) = delete;

    void initialize(const std::filesystem::path& shaders);
    // Returns true when the off-screen target is bound. Zero sizes are not handled here (the
    // camera already rejects them before drawing).
    bool begin(int width, int height, const SceneState& state);
    void finish(const SceneState& state);
    void shutdown();
    bool available() const { return available_; }

    BloomSettings bloom;
    CrtSettings crt;
private:
    bool ensureTargets(int width, int height);
    bool buildTargets(int width, int height, GLenum format);
    void releaseTargets();

    Shader bright_, blur_, composite_;
    GLuint vao_ = 0;
    GLuint msaaFbo_ = 0, msaaColor_ = 0, msaaDepth_ = 0;
    GLuint sceneFbo_ = 0, sceneTexture_ = 0;
    GLuint bloomFbo_[2] = {0, 0}, bloomTexture_[2] = {0, 0};
    int width_ = 0, height_ = 0, bloomWidth_ = 0, bloomHeight_ = 0;
    bool active_ = false, available_ = true;
};
} // namespace sw
