#version 330 core
// Lit-object vertex stage: everything is handed to the fragment stage in world space.
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
uniform mat4 uModel, uView, uProjection;
uniform mat3 uNormalMatrix;   // inverse transpose of the model's upper 3x3
out vec3 vWorldPosition;
out vec3 vNormal;
void main() {
    vec4 world = uModel * vec4(aPosition, 1.0);
    vWorldPosition = world.xyz;
    vNormal = uNormalMatrix * aNormal;
    gl_Position = uProjection * uView * world;
}
