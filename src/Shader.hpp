#pragma once
#include <GL/glew.h>
#include <filesystem>
#include "Math.hpp"

namespace sw {
class Shader {
public:
    Shader()=default;
    ~Shader() { shutdown(); }
    Shader(const Shader&)=delete;
    Shader& operator=(const Shader&)=delete;
    void initialize(const std::filesystem::path& vertex,const std::filesystem::path& fragment);
    void shutdown();
    void use() const { glUseProgram(program_); }
    void set(const char* name,const Mat4& value) const;
    void set(const char* name,const Mat3& value) const;
    void set(const char* name,Vec3 value) const;
    void set(const char* name,float value) const;
private:
    GLint location(const char* name) const;
    GLuint program_=0;
};
} // namespace sw
