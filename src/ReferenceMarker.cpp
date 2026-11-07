#include "ReferenceMarker.hpp"
#include <vector>

namespace sw {
void ReferenceMarker::initialize(const std::filesystem::path& directory) {
    shutdown(); shader_.initialize(directory/"reference.vert",directory/"reference.frag");
    const Vec3 p[]{ {-0.8f,0.05f,-1.5f},{0.8f,0.05f,-1.5f},{0.8f,0.85f,-1.5f},{-0.8f,0.85f,-1.5f},
                    {-0.8f,0.05f,1.5f},{0.8f,0.05f,1.5f},{0.8f,0.85f,1.5f},{-0.8f,0.85f,1.5f} };
    const int faces[6][4]={{0,3,2,1},{4,5,6,7},{0,4,7,3},{1,2,6,5},{3,7,6,2},{0,1,5,4}};
    const Vec3 colors[]={{0.10f,0.20f,0.29f},{0.20f,0.12f,0.33f},{0.09f,0.29f,0.37f},
                         {0.29f,0.10f,0.31f},{0.21f,0.39f,0.46f},{0.04f,0.05f,0.08f}};
    std::vector<float> data;
    for(int face=0;face<6;++face) for(int corner : {0,1,2,0,2,3}) {
        const Vec3 v=p[faces[face][corner]],c=colors[face];
        data.insert(data.end(),{v.x,v.y,v.z,c.x,c.y,c.z});
    }
    glGenVertexArrays(1,&vao_); glGenBuffers(1,&vbo_);
    glBindVertexArray(vao_); glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(data.size()*sizeof(float)),data.data(),GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),nullptr);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),reinterpret_cast<void*>(3*sizeof(float)));
    glBindVertexArray(0);
}
void ReferenceMarker::render(const RenderContext& ctx) const {
    shader_.use(); shader_.set("uModel",Mat4::identity()); shader_.set("uView",ctx.view); shader_.set("uProjection",ctx.projection);
    glBindVertexArray(vao_); glDrawArrays(GL_TRIANGLES,0,36); glBindVertexArray(0);
}
void ReferenceMarker::shutdown() {
    if(vbo_) glDeleteBuffers(1,&vbo_);
    if(vao_) glDeleteVertexArrays(1,&vao_);
    vao_=vbo_=0; shader_.shutdown();
}
}
