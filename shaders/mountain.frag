#version 330 core

in vec3 vNormal;
out vec4 fragColor;

uniform vec3 uBaseColor;
uniform vec3 uLightDirection;

void main() {
    // Flat facet shading only; Nursalim's common lighting replaces this in the integrated build.
    float facet = clamp(dot(normalize(vNormal), normalize(uLightDirection)), 0.0, 1.0);
    vec3 color = uBaseColor * (0.45 + 0.55 * facet);
    fragColor = vec4(color, 1.0);
}
