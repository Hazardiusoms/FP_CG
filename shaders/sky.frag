#version 330 core

in vec2 vUv;
out vec4 fragColor;

const vec3 HORIZON_COLOR = vec3(0.45, 0.08, 0.35);
const vec3 ZENITH_COLOR = vec3(0.02, 0.00, 0.12);

void main() {
    float t = clamp(vUv.y, 0.0, 1.0);
    fragColor = vec4(mix(HORIZON_COLOR, ZENITH_COLOR, t), 1.0);
}
