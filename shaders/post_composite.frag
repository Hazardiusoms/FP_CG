#version 330 core
// Final pass to the screen: scene + blurred bloom, then optional CRT scanlines and vignette.
in vec2 vUv;
uniform sampler2D uScene;
uniform sampler2D uBloom;
uniform float uBloomAmount;      // 0 when bloom is off
uniform float uCrt;              // 0 or 1
uniform float uScanlinePeriod;   // whole pixels
uniform float uScanlineDepth;
uniform float uScanlineGain;     // brightness compensation for the dark gaps
uniform float uVignette;
out vec4 fragColor;
void main() {
    vec3 color = texture(uScene, vUv).rgb + texture(uBloom, vUv).rgb * uBloomAmount;
    if (uCrt > 0.5) {
        float phase = fract((gl_FragCoord.y - 0.5) / uScanlinePeriod);
        float gap = 0.5 - 0.5 * cos(6.2831853 * phase);          // 0 on the line, 1 mid-gap
        float scan = 1.0 - uScanlineDepth * gap;
        vec2 d = vUv - 0.5;
        float vignette = 1.0 - uVignette * dot(d, d);
        color *= scan * uScanlineGain * vignette;
    }
    fragColor = vec4(color, 1.0);
}
