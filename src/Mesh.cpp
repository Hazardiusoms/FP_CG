#include "Mesh.hpp"
#include <stdexcept>
#include <limits>
namespace sw {
void Mesh::initialize(const std::vector<float>& v,std::size_t stride,
                      const std::vector<std::uint32_t>& indices,const std::vector<Attribute>& attrs) {
    if(!stride || v.empty() || v.size()%stride || indices.empty() || indices.size()%3)
        throw std::invalid_argument("Invalid triangle mesh layout");
    for(auto i:indices) if(i>=v.size()/stride) throw std::invalid_argument("Mesh index out of range");
    for(auto a:attrs) if(a.components<1 || a.components>4 || a.offsetFloats+a.components>stride)
        throw std::invalid_argument("Mesh attribute outside vertex stride");
    if(indices.size()>static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()))
        throw std::invalid_argument("Mesh index count too large");
    shutdown(); count_=static_cast<GLsizei>(indices.size());
    glGenVertexArrays(1,&vao_); glGenBuffers(1,&vbo_); glGenBuffers(1,&ebo_);
    glBindVertexArray(vao_); glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,v.size()*sizeof(float),v.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(std::uint32_t),indices.data(),GL_STATIC_DRAW);
    for(auto a:attrs){
        glEnableVertexAttribArray(a.location);
        glVertexAttribPointer(a.location,a.components,GL_FLOAT,GL_FALSE,static_cast<GLsizei>(stride*sizeof(float)),reinterpret_cast<void*>(a.offsetFloats*sizeof(float)));
    }
    glBindVertexArray(0);
}
void Mesh::draw() const { glBindVertexArray(vao_); glDrawElements(GL_TRIANGLES,count_,GL_UNSIGNED_INT,nullptr); glBindVertexArray(0); }
void Mesh::shutdown(){
    if(ebo_)glDeleteBuffers(1,&ebo_);if(vbo_)glDeleteBuffers(1,&vbo_);if(vao_)glDeleteVertexArrays(1,&vao_);
    vao_=vbo_=ebo_=0;count_=0;
}
}
