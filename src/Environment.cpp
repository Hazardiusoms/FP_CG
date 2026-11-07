#include "Environment.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Cannot open shader file: " << path << "\n";
        return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& path) {
    const std::string source = readFile(path);
    if (source.empty()) {
        return 0;
    }

    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error (" << path << "):\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Program link error:\n" << log << "\n";
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

}

bool Environment::init() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "shaders/sky.vert");
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "shaders/sky.frag");
    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    skyProgram_ = linkProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (skyProgram_ == 0) {
        return false;
    }

    glGenVertexArrays(1, &skyVao_);
    return true;
}

void Environment::draw() {
    glDepthMask(GL_FALSE);
    glUseProgram(skyProgram_);
    glBindVertexArray(skyVao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}

void Environment::shutdown() {
    glDeleteVertexArrays(1, &skyVao_);
    glDeleteProgram(skyProgram_);
    skyVao_ = 0;
    skyProgram_ = 0;
}
