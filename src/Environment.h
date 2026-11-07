#pragma once

#include <GL/glew.h>

class Environment {
public:
    bool init();
    void draw();
    void shutdown();

private:
    GLuint skyProgram_ = 0;
    GLuint skyVao_ = 0;
};
