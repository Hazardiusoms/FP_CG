#include "Road.hpp"
namespace sw {
void Road::initialize(const std::filesystem::path& directory){
    shutdown();reset();
    shader_.initialize(directory/"road.vert",directory/"road.frag");
    mesh_.initialize({-80,0,-300,-40,-150, -80,0,20,-40,10, 80,0,20,40,10, 80,0,-300,40,-150},
        5,{0,1,2,0,2,3},{{0,3,0},{2,2,3}});
}
void Road::update(float dt,float speed){offset_=advanceGridOffset(offset_,speed,dt);}
void Road::render(const RenderContext& ctx,bool alternatePalette) const{
    shader_.use();shader_.set("uModel",Mat4::identity());
    shader_.set("uView",ctx.view);shader_.set("uProjection",ctx.projection);
    shader_.set("uOffset",static_cast<float>(offset_));shader_.set("uRoadHalfWidth",roadHalfWidth);
    shader_.set("uCameraPosition",ctx.cameraPosition);shader_.set("uAlternatePalette",alternatePalette ? 1.0f : 0.0f);
    mesh_.draw();
}
void Road::shutdown(){mesh_.shutdown();shader_.shutdown();}
}
