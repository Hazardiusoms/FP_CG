#pragma once
#include "Mesh.hpp"
#include "RenderContext.hpp"
#include "SceneState.hpp"
#include "Shader.hpp"

namespace sw {
class Road {
  public:
    Road() = default;
    ~Road() { shutdown(); }
    Road(const Road &) = delete;
    Road &operator=(const Road &) = delete;
    void initialize(const std::filesystem::path &shaderDirectory);
    void update(float deltaTime, float speed);
    void render(const RenderContext &context, bool alternatePalette = false) const;
    // Soft light cones under the lamp heads: additive, drawn late so they blend over everything
    // already in the frame and are hidden by nearer geometry (they write no depth).
    void renderBeams(const RenderContext &context, bool alternatePalette = false) const;
    void reset() {
        offset_ = 0;
        travel_ = 0;
    }
    void shutdown();
    double offset() const { return offset_; }
    // Road travel in metres, 0..12: the phase the lamp posts and their light pools share.
    double travel() const { return travel_; }

  private:
    Shader shader_, roadsideShader_, beamShader_;
    Mesh mesh_, rails_, markers_, lamps_, beams_;
    double offset_ = 0, travel_ = 0;
};
} // namespace sw
