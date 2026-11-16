#version 330 core
// One direction of a separable 9-tap Gaussian. Weights come from the CPU (sum to 1).
in vec2 vUv;
uniform sampler2D uImage;
uniform vec3 uDirection;      // xy: step in texels (1,0) or (0,1); z unused
uniform float uWeights[5];
out vec4 fragColor;
void main() {
    vec2 step = uDirection.xy / vec2(textureSize(uImage, 0));
    vec3 sum = texture(uImage, vUv).rgb * uWeights[0];
    for (int i = 1; i < 5; ++i) {
        sum += texture(uImage, vUv + step * float(i)).rgb * uWeights[i];
        sum += texture(uImage, vUv - step * float(i)).rgb * uWeights[i];
    }
    fragColor = vec4(sum, 1.0);
}
