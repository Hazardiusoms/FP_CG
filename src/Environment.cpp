#include "Environment.hpp"
#include "Math.hpp"
#include "SceneState.hpp"
#include <cmath>
#include <cstddef>
#include <memory>
#include <vector>

namespace sw {
namespace {
// One ridge-like peak: apex at the centre, base corners to the sides and back.
// Position (3) + normal (3) per vertex, with duplicated vertices per face for flat facets.
std::vector<float> peakVertices(float halfWidth,float height,float depth,float skew) {
    const Vec3 apex{skew,height,0};
    const Vec3 left{-halfWidth,0,-depth}, right{halfWidth,0,-depth};
    const Vec3 front{0,0,depth}, back{0,0,-depth};
    std::vector<float> data;
    auto emit=[&](Vec3 a,Vec3 b,Vec3 c){
        const Vec3 n=normalized(cross(b-a,c-a));
        for(Vec3 p:{a,b,c}) data.insert(data.end(),{p.x,p.y,p.z,n.x,n.y,n.z});
    };
    // Four triangular faces around the apex; base corners ring the apex.
    emit(apex,left,front); emit(apex,front,right); emit(apex,right,back); emit(apex,back,left);
    return data;
}
}

void Environment::initialize(const std::filesystem::path& directory) {
    shutdown();
    backgroundShader_.initialize(directory/"background.vert",directory/"background.frag");
    mountainShader_.initialize(directory/"mountain.vert",directory/"mountain.frag");
    glGenVertexArrays(1,&backgroundVao_);

    // Three mountain variants, all low-poly peaks with different widths and heights.
    const float variants[][4]={{3.5f,4.0f,3.0f,0.0f},{2.8f,6.0f,2.5f,0.5f},{4.0f,3.2f,3.5f,-0.4f}};
    for(const auto& v:variants) {
        auto mesh=std::make_unique<Mesh>();
        const auto data=peakVertices(v[0],v[1],v[2],v[3]);
        std::vector<std::uint32_t> indices(data.size()/6);
        for(std::uint32_t i=0;i<indices.size();++i) indices[i]=i;
        mesh->initialize(data,6,indices,{{0,3,0},{1,3,3}});
        meshes_.push_back(std::move(mesh));
    }
    buildSections();
}

void Environment::buildSections() {
    sections_.clear();
    // Left and right rows, beyond the road edge with a visible gap, spaced along -Z.
    for(int row=0;row<kRowsPerSide;++row) {
        const float z=-sectionLength*row;
        const std::size_t variant=static_cast<std::size_t>(row)%meshes_.size();
        sections_.push_back({z,-roadHalfWidth-9.0f,variant});
        sections_.push_back({z-sectionLength*0.5f,roadHalfWidth+9.0f,(variant+1)%meshes_.size()});
    }
}

void Environment::update(float deltaTime,float speed) {
    // Scenery moves toward +Z, matching the road grid. Sections that pass behind the camera
    // are moved back by one full period so the landscape never ends.
    const float period=sectionLength*kRowsPerSide;
    for(auto& s:sections_) {
        s.z+=speed*deltaTime;
        if(s.z>cameraRecycleZ) s.z-=period;
    }
}

void Environment::reset() {
    buildSections();
}

void Environment::renderBackground(const RenderContext& ctx) const {
    if(!backgroundVao_) return;
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    backgroundShader_.use();
    backgroundShader_.set("uCameraForward",ctx.cameraForward);
    backgroundShader_.set("uCameraRight",ctx.cameraRight);
    backgroundShader_.set("uCameraUp",ctx.cameraUp);
    backgroundShader_.set("uTanHalfFov",std::tan(ctx.verticalFov*0.5f));
    backgroundShader_.set("uAspect",static_cast<float>(ctx.framebufferWidth)/static_cast<float>(ctx.framebufferHeight));
    backgroundShader_.set("uSunDirection",normalized({0.0f,0.03f,-1.0f}));
    backgroundShader_.set("uSunRadius",0.22f);
    backgroundShader_.set("uStripeCount",6.0f);
    glBindVertexArray(backgroundVao_);
    glDrawArrays(GL_TRIANGLES,0,3);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void Environment::renderMountains(const RenderContext& ctx) const {
    mountainShader_.use();
    mountainShader_.set("uView",ctx.view);
    mountainShader_.set("uProjection",ctx.projection);
    mountainShader_.set("uLightDirection",normalized({0.3f,0.8f,0.5f}));
    for(const auto& s:sections_) {
        mountainShader_.set("uModel",translation({s.x,0,s.z}));
        mountainShader_.set("uBaseColor",s.variant==0 ? Vec3{0.36f,0.12f,0.42f} : s.variant==1 ? Vec3{0.52f,0.18f,0.46f} : Vec3{0.28f,0.10f,0.36f});
        meshes_[s.variant]->draw();
    }
}

void Environment::shutdown() {
    meshes_.clear();
    sections_.clear();
    if(backgroundVao_) glDeleteVertexArrays(1,&backgroundVao_);
    backgroundVao_=0;
    backgroundShader_.shutdown();
    mountainShader_.shutdown();
}
}
