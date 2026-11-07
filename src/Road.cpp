#include "Road.hpp"
#include <cstddef>

namespace sw {
void Road::initialize(const std::filesystem::path& directory) {
    shutdown(); reset();
    shader_.initialize(directory/"road.vert",directory/"road.frag");
    struct Vertex { float x,y,z,u,v; };
    // UV v=z/gridWorldSize; subtracting a growing offset moves lines along +Z.
    const Vertex vertices[]{
        {-80,0,-300,-40,-150}, {-80,0,20,-40,10}, {80,0,20,40,10},
        {-80,0,-300,-40,-150}, {80,0,20,40,10}, {80,0,-300,40,-150}
    };
    glGenVertexArrays(1,&vao_); glGenBuffers(1,&vbo_);
    glBindVertexArray(vao_); glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);
    glEnableVertexAttribArray(2); glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex,u)));
    glBindVertexArray(0);
}
void Road::update(float dt,float speed) { offset_=advanceGridOffset(offset_,speed,dt); }
void Road::render(const RenderContext& ctx) const {
    shader_.use();
    shader_.set("uView",ctx.view); shader_.set("uProjection",ctx.projection);
    shader_.set("uOffset",static_cast<float>(offset_));
    shader_.set("uRoadHalfWidth",roadHalfWidth);
    shader_.set("uCameraPosition",ctx.cameraPosition);
    glBindVertexArray(vao_); glDrawArrays(GL_TRIANGLES,0,6); glBindVertexArray(0);
}
void Road::shutdown() {
    if(vbo_) glDeleteBuffers(1,&vbo_);
    if(vao_) glDeleteVertexArrays(1,&vao_);
    vao_=vbo_=0; shader_.shutdown();
}
} // namespace sw
