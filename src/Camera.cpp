#include "Camera.hpp"

namespace sw {
bool Camera::update(const SceneState& scene,int width,int height,RenderContext& ctx) const {
    ctx.framebufferWidth=width; ctx.framebufferHeight=height;
    if (width<=0 || height<=0) return false;
    const bool follow=scene.camera==CameraMode::Follow;
    ctx.cameraPosition=follow ? Vec3{scene.carX*0.35f,3.0f,7.0f} : Vec3{9.0f,7.0f,11.0f};
    const Vec3 target=follow ? Vec3{scene.carX*0.2f,0.6f,-8.0f} : Vec3{0,0,-8};
    ctx.cameraForward=normalized(target-ctx.cameraPosition);
    ctx.cameraRight=normalized(cross(ctx.cameraForward,{0,1,0}));
    ctx.cameraUp=cross(ctx.cameraRight,ctx.cameraForward);
    ctx.view=lookAt(ctx.cameraPosition,target,{0,1,0});
    ctx.projection=perspective(ctx.verticalFov,static_cast<float>(width)/height,0.1f,400.0f);
    return true;
}
} // namespace sw
