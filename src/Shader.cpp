#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace sw {
namespace {
GLuint compile(const std::filesystem::path& path,GLenum stage) {
    std::ifstream file(path);
    if(!file) throw std::runtime_error("Cannot open shader: "+path.string());
    std::ostringstream buffer; buffer<<file.rdbuf();
    const std::string source=buffer.str(); const char* data=source.c_str();
    const GLuint shader=glCreateShader(stage);
    if (!shader) throw std::runtime_error("Cannot allocate shader: "+path.string());
    glShaderSource(shader,1,&data,nullptr); glCompileShader(shader);
    GLint ok=0; glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);
    if (!ok) {
        GLint size=0; glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&size);
        std::vector<char> log(static_cast<std::size_t>(size>0 ? size : 1));
        glGetShaderInfoLog(shader,static_cast<GLsizei>(log.size()),nullptr,log.data());
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed: "+path.string()+"\n"+log.data());
    }
    return shader;
}
}
void Shader::initialize(const std::filesystem::path& vertex,const std::filesystem::path& fragment) {
    shutdown();
    GLuint vs=0,fs=0;
    try {
        vs=compile(vertex,GL_VERTEX_SHADER); fs=compile(fragment,GL_FRAGMENT_SHADER);
        program_=glCreateProgram(); glAttachShader(program_,vs); glAttachShader(program_,fs);
        glLinkProgram(program_);
        GLint ok=0; glGetProgramiv(program_,GL_LINK_STATUS,&ok);
        if(!ok) {
            GLint size=0; glGetProgramiv(program_,GL_INFO_LOG_LENGTH,&size);
            std::vector<char> log(static_cast<std::size_t>(size>0 ? size : 1));
            glGetProgramInfoLog(program_,static_cast<GLsizei>(log.size()),nullptr,log.data());
            throw std::runtime_error("Shader link failed: "+vertex.string()+" + "+fragment.string()+"\n"+log.data());
        }
        glDetachShader(program_,vs); glDetachShader(program_,fs);
        glDeleteShader(vs); glDeleteShader(fs);
    } catch(...) {
        if(vs) glDeleteShader(vs);
        if(fs) glDeleteShader(fs);
        shutdown(); throw;
    }
}
void Shader::shutdown() { if(program_) glDeleteProgram(program_); program_=0; }
GLint Shader::location(const char* name) const {
    const GLint loc=glGetUniformLocation(program_,name);
    if(loc<0) throw std::runtime_error(std::string("Missing active uniform: ")+name);
    return loc;
}
void Shader::set(const char* name,const Mat4& m) const { glUniformMatrix4fv(location(name),1,GL_FALSE,m.data()); }
void Shader::set(const char* name,const Mat3& m) const { glUniformMatrix3fv(location(name),1,GL_FALSE,m.data()); }
void Shader::set(const char* name,Vec3 v) const { glUniform3f(location(name),v.x,v.y,v.z); }
void Shader::set(const char* name,float v) const { glUniform1f(location(name),v); }
} // namespace sw
