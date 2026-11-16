#include "PostProcessing.hpp"
#include <algorithm>
#include <iostream>
#include <string>

namespace sw {
namespace {
GLuint makeTexture(int width, int height, GLenum format) {
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return texture;
}
GLuint makeFramebuffer(GLuint texture) {
    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    return fbo;
}
bool complete() { return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE; }
void deleteTexture(GLuint& id) { if (id) glDeleteTextures(1, &id); id = 0; }
void deleteFramebuffer(GLuint& id) { if (id) glDeleteFramebuffers(1, &id); id = 0; }
void deleteRenderbuffer(GLuint& id) { if (id) glDeleteRenderbuffers(1, &id); id = 0; }
} // namespace

void PostProcessing::initialize(const std::filesystem::path& shaders) {
    shutdown();
    const auto vertex = shaders / "post_fullscreen.vert";
    bright_.initialize(vertex, shaders / "post_bright.frag");
    blur_.initialize(vertex, shaders / "post_blur.frag");
    composite_.initialize(vertex, shaders / "post_composite.frag");
    glGenVertexArrays(1, &vao_);   // core profile needs a bound VAO even with no vertex data
    available_ = true;
}

void PostProcessing::releaseTargets() {
    deleteFramebuffer(msaaFbo_); deleteRenderbuffer(msaaColor_); deleteRenderbuffer(msaaDepth_);
    deleteFramebuffer(sceneFbo_); deleteTexture(sceneTexture_);
    for (int i = 0; i < 2; ++i) { deleteFramebuffer(bloomFbo_[i]); deleteTexture(bloomTexture_[i]); }
    width_ = height_ = bloomWidth_ = bloomHeight_ = 0;
}

bool PostProcessing::buildTargets(int width, int height, GLenum format) {
    while (glGetError() != GL_NO_ERROR) {}
    GLint maxSamples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    const int samples = std::min(4, static_cast<int>(maxSamples));
    bloomWidth_ = halfSize(width);
    bloomHeight_ = halfSize(height);

    glGenFramebuffers(1, &msaaFbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo_);
    glGenRenderbuffers(1, &msaaColor_);
    glBindRenderbuffer(GL_RENDERBUFFER, msaaColor_);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaColor_);
    glGenRenderbuffers(1, &msaaDepth_);
    glBindRenderbuffer(GL_RENDERBUFFER, msaaDepth_);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, msaaDepth_);
    if (!complete()) return false;

    sceneTexture_ = makeTexture(width, height, format);
    sceneFbo_ = makeFramebuffer(sceneTexture_);
    if (!complete()) return false;
    for (int i = 0; i < 2; ++i) {
        bloomTexture_[i] = makeTexture(bloomWidth_, bloomHeight_, format);
        bloomFbo_[i] = makeFramebuffer(bloomTexture_[i]);
        if (!complete()) return false;
    }
    return glGetError() == GL_NO_ERROR;
}

bool PostProcessing::ensureTargets(int width, int height) {
    if (width == width_ && height == height_ && msaaFbo_) return true;
    releaseTargets();
    // Half-float keeps highlights above 1.0 for a better bloom; plain 8-bit is the fallback.
    for (const GLenum format : {GL_RGBA16F, GL_RGBA8}) {
        if (buildTargets(width, height, format)) { width_ = width; height_ = height; return true; }
        releaseTargets();
    }
    return false;
}

bool PostProcessing::begin(int width, int height, const SceneState& state) {
    active_ = false;
    if (!available_ || !(state.bloomEnabled || state.crtEnabled) || width <= 0 || height <= 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    if (!ensureTargets(width, height)) {
        available_ = false;
        std::cerr << "Post-processing targets could not be created; bloom and CRT are disabled.\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo_);
    active_ = true;
    return true;
}

void PostProcessing::finish(const SceneState& state) {
    if (!active_) return;
    active_ = false;
    glDisable(GL_DEPTH_TEST); glDisable(GL_BLEND); glDisable(GL_CULL_FACE);
    // Resolve the multisampled scene into a texture that shaders can read.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFbo_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sceneFbo_);
    glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindVertexArray(vao_);

    if (state.bloomEnabled) {
        glViewport(0, 0, bloomWidth_, bloomHeight_);
        bright_.use();
        bright_.setInt("uScene", 0);
        bright_.set("uThreshold", bloom.threshold);
        bright_.set("uKnee", bloom.knee);
        glBindFramebuffer(GL_FRAMEBUFFER, bloomFbo_[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneTexture_);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        blur_.use();
        blur_.setInt("uImage", 0);
        const auto weights = gaussianWeights(bloom.sigma);
        for (int i = 0; i < blurTapCount; ++i) {
            const std::string name = "uWeights[" + std::to_string(i) + "]";
            blur_.set(name.c_str(), weights[i]);
        }
        for (int i = 0; i < std::max(1, bloom.blurIterations); ++i) {
            blur_.set("uDirection", Vec3{1, 0, 0});      // horizontal: bloom 0 -> bloom 1
            glBindFramebuffer(GL_FRAMEBUFFER, bloomFbo_[1]);
            glBindTexture(GL_TEXTURE_2D, bloomTexture_[0]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            blur_.set("uDirection", Vec3{0, 1, 0});      // vertical: bloom 1 -> bloom 0
            glBindFramebuffer(GL_FRAMEBUFFER, bloomFbo_[0]);
            glBindTexture(GL_TEXTURE_2D, bloomTexture_[1]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width_, height_);
    composite_.use();
    composite_.setInt("uScene", 0);
    composite_.setInt("uBloom", 1);
    composite_.set("uBloomAmount", state.bloomEnabled ? bloom.intensity : 0.0f);
    composite_.set("uCrt", state.crtEnabled ? 1.0f : 0.0f);
    composite_.set("uScanlinePeriod", static_cast<float>(scanlinePeriod(height_)));
    composite_.set("uScanlineDepth", crt.scanlineDepth);
    composite_.set("uScanlineGain", scanlineCompensation(crt.scanlineDepth));
    composite_.set("uVignette", crt.vignette);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture_);
    glActiveTexture(GL_TEXTURE1);
    // With bloom off this texture is never written; bind the scene instead so no stale data is read.
    glBindTexture(GL_TEXTURE_2D, state.bloomEnabled ? bloomTexture_[0] : sceneTexture_);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    // The HUD and the next frame expect depth testing to be on.
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

void PostProcessing::shutdown() {
    releaseTargets();
    if (vao_) glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
    bright_.shutdown(); blur_.shutdown(); composite_.shutdown();
}
} // namespace sw
