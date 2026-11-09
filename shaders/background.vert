#version 330 core

// Full-screen triangle generated from gl_VertexID; no vertex buffer is needed.
// Positions are in normalized device coordinates; depth is set to the far plane.
out vec2 vNdc;

void main() {
    vec2 pos = vec2((gl_VertexID & 1) * 4.0 - 1.0, (gl_VertexID >> 1) * 4.0 - 1.0);
    vNdc = pos;
    gl_Position = vec4(pos, 0.9999, 1.0);
}
