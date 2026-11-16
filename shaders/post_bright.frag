#version 330 core
// Bloom bright pass at half resolution: a 4-tap box downsample (bilinear taps cover 4x4 source
// texels) followed by a soft-knee threshold on the brightest colour channel.
in vec2 vUv;
uniform sampler2D uScene;
uniform float uThreshold;
uniform float uKnee;
out vec4 fragColor;
void main() {
    vec2 texel = 1.0 / vec2(textureSize(uScene, 0));
    vec3 color = 0.25 * (texture(uScene, vUv + texel * vec2(-1.0, -1.0)).rgb
                       + texture(uScene, vUv + texel * vec2( 1.0, -1.0)).rgb
                       + texture(uScene, vUv + texel * vec2(-1.0,  1.0)).rgb
                       + texture(uScene, vUv + texel * vec2( 1.0,  1.0)).rgb);
    float peak = max(color.r, max(color.g, color.b));
    // Quadratic knee: smooth ramp from threshold-knee to threshold+knee, linear above it.
    float soft = clamp(peak - uThreshold + uKnee, 0.0, 2.0 * uKnee);
    soft = soft * soft / (4.0 * uKnee + 1e-4);
    float contribution = max(soft, peak - uThreshold) / max(peak, 1e-4);
    fragColor = vec4(color * contribution, 1.0);
}
