#pragma once
#include <GL/glew.h>
#include <vector>
#include <cstddef>
#include <cstdint>
namespace sw {
struct Attribute { GLuint location; GLint components; std::size_t offsetFloats; };
class Mesh {
public:
    Mesh()=default;
    ~Mesh(){ shutdown(); }
    Mesh(const Mesh&)=delete;
    Mesh& operator=(const Mesh&)=delete;
    void initialize(const std::vector<float>& vertices, std::size_t strideFloats,
                    const std::vector<std::uint32_t>& indices, const std::vector<Attribute>& attributes);
    void draw() const;
    void shutdown();
private:
    GLuint vao_=0,vbo_=0,ebo_=0;
    GLsizei count_=0;
};
}
