#version 330 core
layout(location=0) in vec3 aPosition;
layout(location=2) in vec2 aUv;
uniform mat4 uView;
uniform mat4 uProjection;
out vec2 vUv;
out vec3 vWorldPosition;
void main() {
    vUv=aUv;
    vWorldPosition=aPosition;
    gl_Position=uProjection*uView*vec4(aPosition,1.0);
}
