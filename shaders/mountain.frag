#version 330 core

in vec3 vNormal;
in float vRidge;
in float vHeight;
in vec3 vWorld;
out vec4 fragColor;

uniform vec3 uBaseColor;
uniform vec3 uLightDirection;
uniform vec3 uFogColor;
uniform vec3 uRimColor;
uniform vec3 uSnowColor;
uniform float uDistance;
uniform float uPalm;
uniform float uGround;

// Cheap hash noise for the procedural ground texture.
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void main() {
    vec3 n = normalize(vNormal);

    float facing = dot(n, normalize(uLightDirection));
    float lit = smoothstep(-0.2, 0.6, facing);
    vec3 color = uBaseColor * mix(0.45, 1.15, lit);

    // Palm fronds and flat signs: flat tone, no rim or fog.
    if (uPalm > 0.5) {
        color = uBaseColor * mix(0.7, 1.1, lit);
        fragColor = vec4(color, 1.0);
        return;
    }

    color *= mix(0.55, 1.0, vRidge);
    float snow = smoothstep(0.82, 1.0, vRidge) * lit;
    color = mix(color, uSnowColor, snow * 0.35);

    float rim = smoothstep(0.85, 1.0, vRidge) * smoothstep(0.2, 0.6, n.y);
    color = mix(color, uRimColor, rim * 0.45);

    float fog = clamp((uDistance - 14.0) / 60.0, 0.0, 0.85);
    color = mix(color, uFogColor, fog);

    fragColor = vec4(color, 1.0);
}
