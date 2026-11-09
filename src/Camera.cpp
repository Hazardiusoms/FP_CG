#include "Camera.hpp"

namespace sw {
bool Camera::update(const SceneState &scene, int width, int height, RenderContext &ctx) const {
    ctx.framebufferWidth = width;
    ctx.framebufferHeight = height;
    if (width <= 0 || height <= 0)
        return false;
    const bool follow = scene.camera == CameraMode::Follow;
    ctx.cameraPosition =
        follow ? Vec3{scene.followX * 0.65f, 2.6f, 7.5f}
               : Vec3{scene.carX +
                          scene.orbitRadius * std::cos(scene.orbitPitch) * std::sin(scene.orbitYaw),
                      0.5f + scene.orbitRadius * std::sin(scene.orbitPitch),
                      scene.orbitRadius * std::cos(scene.orbitPitch) * std::cos(scene.orbitYaw)};
    const Vec3 target =
        follow ? Vec3{scene.followX * 0.4f, 0.65f, -10.0f} : Vec3{scene.carX, 0.5f, 0};
    ctx.cameraForward = normalized(target - ctx.cameraPosition);
    ctx.cameraRight = normalized(cross(ctx.cameraForward, {0, 1, 0}));
    ctx.cameraUp = cross(ctx.cameraRight, ctx.cameraForward);
    ctx.view = lookAt(ctx.cameraPosition, target, {0, 1, 0});
    ctx.projection = perspective(ctx.verticalFov, static_cast<float>(width) / height, 0.1f, 400.0f);
    return true;
}
} // namespace sw
