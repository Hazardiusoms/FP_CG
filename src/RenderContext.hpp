#pragma once
#include "Math.hpp"

namespace sw {
struct RenderContext {
    Mat4 view = Mat4::identity();
    Mat4 projection = Mat4::identity();
    Vec3 cameraPosition{};
    Vec3 cameraForward{};
    Vec3 cameraRight{};
    Vec3 cameraUp{};
    float verticalFov = radians(55.0f);
    int framebufferWidth = 0;
    int framebufferHeight = 0;
};
} // namespace sw
