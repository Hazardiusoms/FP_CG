#version 330 core

in vec2 vNdc;
out vec4 fragColor;

uniform vec3 uCameraForward;
uniform vec3 uCameraRight;
uniform vec3 uCameraUp;
uniform float uTanHalfFov;
uniform float uAspect;
uniform vec3 uSunDirection;
uniform float uSunRadius;
uniform float uStripeCount;
uniform float uTime;

const vec3 ZENITH_COLOR = vec3(0.02, 0.00, 0.12);
const vec3 HORIZON_COLOR = vec3(0.45, 0.08, 0.35);
const vec3 SUN_TOP = vec3(1.00, 0.85, 0.35);
const vec3 SUN_BOTTOM = vec3(1.00, 0.25, 0.45);
const vec3 GLOW_COLOR = vec3(1.00, 0.35, 0.45);

float hash3(vec3 p) {
    return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453);
}

void main() {
    // Ray from the shared camera basis. Camera translation is ignored, so the sky stays distant.
    vec3 dir = normalize(uCameraForward
        + uCameraRight * (vNdc.x * uAspect * uTanHalfFov)
        + uCameraUp * (vNdc.y * uTanHalfFov));

    float horizon = clamp(dir.y * 3.0 + 0.5, 0.0, 1.0);
    vec3 color = mix(HORIZON_COLOR, ZENITH_COLOR, horizon);

    vec3 sunDir = normalize(uSunDirection);
    float angle = acos(clamp(dot(dir, sunDir), -1.0, 1.0));
    // 0 close to the sun, 1 far from it: stars, aurora and shooting stars fade out near the glow.
    float sunClear = smoothstep(0.25, 0.6, angle);

    // Stars: each cell of a direction grid may hold one star with its own twinkle rate.
    if (dir.y > 0.02) {
        vec3 g = dir * 80.0;
        vec3 cell = floor(g);
        float seed = hash3(cell);
        if (seed > 0.97) {
            vec3 centre = vec3(hash3(cell + 1.7), hash3(cell + 4.3), hash3(cell + 9.1)) * 0.4 + 0.3;
            float d = length(fract(g) - centre);
            float twinkle = 0.7 + 0.3 * sin(uTime * (1.5 + 3.0 * seed) + seed * 40.0);
            float star = smoothstep(0.45, 0.0, d) * twinkle;
            star *= smoothstep(0.02, 0.2, dir.y) * sunClear;
            color += vec3(0.90, 0.95, 1.00) * star * 1.4;
        }
    }

    // Aurora: soft wavy ribbons just above the horizon, slowly drifting around the sky.
    float az = atan(dir.x, -dir.z);
    float wave = sin(az * 5.0 + sin(az * 3.0 + uTime * 0.25) * 1.6 + uTime * 0.15);
    float ribbonY = 0.16 + 0.05 * wave;
    float band = exp(-pow((dir.y - ribbonY) / 0.03, 2.0));
    float rays = 0.6 + 0.4 * sin(az * 40.0 + uTime * 0.8 + wave * 3.0);
    vec3 auroraColor = mix(vec3(0.20, 1.00, 0.70), vec3(0.85, 0.25, 0.95),
                           0.5 + 0.5 * sin(az * 2.0 + uTime * 0.1));
    color += auroraColor * band * rays * 0.22 * (0.4 + 0.6 * sunClear);

    // Sun glow outside the disc; it fades with angular distance from the sun edge.
    float outside = max(angle - uSunRadius, 0.0);
    color += GLOW_COLOR * 0.30 * exp(-outside * 9.0);

    // Disc coordinate: y = -1 at the bottom edge, +1 at the top edge, measured on the camera's up axis.
    float y = (dot(dir, uCameraUp) - dot(sunDir, uCameraUp)) / uSunRadius;
    float disc = 1.0 - smoothstep(uSunRadius * 0.99, uSunRadius, angle);
    if (disc > 0.0) {
        vec3 sunColor = mix(SUN_BOTTOM, SUN_TOP, clamp(y * 0.5 + 0.5, 0.0, 1.0));
        // Horizontal bands only in the lower half of the disc. Gaps show the sky behind them.
        float lower = step(y, 0.0);
        float band2 = step(0.35, fract(-y * uStripeCount));
        float mask = mix(1.0, band2, lower);
        color = mix(color, sunColor, disc * mask);
    }

    fragColor = vec4(color, 1.0);
}
