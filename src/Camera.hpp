#pragma once
#include "RenderContext.hpp"
#include "SceneState.hpp"

namespace sw {
class Camera {
public:
    // Returns false for minimized/zero-sized framebuffers without dividing by zero.
    bool update(const SceneState& scene, int width, int height, RenderContext& context) const;
};
} // namespace sw
