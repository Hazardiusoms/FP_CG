#version 330 core
in vec3 vNormal;
uniform vec3 uBaseColor, uEmissive;
out vec4 fragColor;
void main() {
    // Temporary orientation shading for geometry review, NOT the R4 lighting model.
    float facet=0.45+0.55*max(dot(normalize(vNormal),normalize(vec3(-0.3,0.8,0.5))),0.0);
    fragColor=vec4(uBaseColor*facet+uEmissive,1.0);
}
