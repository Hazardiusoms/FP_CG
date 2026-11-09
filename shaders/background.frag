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

const vec3 ZENITH_COLOR = vec3(0.02, 0.00, 0.12);
const vec3 HORIZON_COLOR = vec3(0.45, 0.08, 0.35);
const vec3 SUN_TOP = vec3(1.00, 0.85, 0.35);
const vec3 SUN_BOTTOM = vec3(1.00, 0.25, 0.45);
const vec3 GLOW_COLOR = vec3(1.00, 0.35, 0.45);

void main() {
    // Ray from the shared camera basis. Camera translation is ignored, so the sky stays distant.
    vec3 dir = normalize(uCameraForward
        + uCameraRight * (vNdc.x * uAspect * uTanHalfFov)
        + uCameraUp * (vNdc.y * uTanHalfFov));

    float horizon = clamp(dir.y * 3.0 + 0.5, 0.0, 1.0);
    vec3 color = mix(HORIZON_COLOR, ZENITH_COLOR, horizon);

    vec3 sunDir = normalize(uSunDirection);
    float angle = acos(clamp(dot(dir, sunDir), -1.0, 1.0));

    // Soft glow outside the disc; it fades with angular distance from the sun edge.
    float outside = max(angle - uSunRadius, 0.0);
    color += GLOW_COLOR * 0.30 * exp(-outside * 9.0);

    // Disc coordinate: y = -1 at the bottom edge, +1 at the top edge, measured on the camera's up axis.
    float y = (dot(dir, uCameraUp) - dot(sunDir, uCameraUp)) / uSunRadius;
    float disc = 1.0 - smoothstep(uSunRadius * 0.99, uSunRadius, angle);
    if (disc > 0.0) {
        vec3 sunColor = mix(SUN_BOTTOM, SUN_TOP, clamp(y * 0.5 + 0.5, 0.0, 1.0));
        // Horizontal bands only in the lower half of the disc. Gaps show the sky behind them.
        float lower = step(y, 0.0);
        float band = step(0.35, fract(-y * uStripeCount));
        float mask = mix(1.0, band, lower);
        color = mix(color, sunColor, disc * mask);
    }

    fragColor = vec4(color, 1.0);
}
