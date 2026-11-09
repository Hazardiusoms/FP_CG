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

void main() {
    // Ray direction from the shared camera basis; camera translation is ignored, so the sky is distant.
    vec3 dir = normalize(uCameraForward
        + uCameraRight * (vNdc.x * uAspect * uTanHalfFov)
        + uCameraUp * (vNdc.y * uTanHalfFov));

    float horizon = clamp(dir.y * 3.0 + 0.5, 0.0, 1.0);
    vec3 color = mix(HORIZON_COLOR, ZENITH_COLOR, horizon);

    // Circular sun disc: angular distance from the sun direction, measured in radians.
    float angle = acos(clamp(dot(dir, normalize(uSunDirection)), -1.0, 1.0));
    float disc = 1.0 - smoothstep(uSunRadius * 0.98, uSunRadius, angle);
    if (disc > 0.0) {
        // Vertical position inside the disc: 0 at the bottom edge, 1 at the top.
        float sunUp = dot(normalize(uSunDirection), uCameraUp);
        float rayUp = dot(dir, uCameraUp);
        float vertical = clamp(0.5 + (rayUp - sunUp) / (uSunRadius * 2.0), 0.0, 1.0);
        vec3 sunColor = mix(SUN_BOTTOM, SUN_TOP, vertical);
        // Horizontal stripes cut the lower half of the disc, the classic synthwave look.
        float stripe = step(0.45, fract(vertical * uStripeCount));
        float mask = mix(1.0, stripe, step(vertical, 0.5));
        color = mix(color, sunColor, disc * mask);
        color = mix(color, ZENITH_COLOR, disc * (1.0 - mask));
    }

    fragColor = vec4(color, 1.0);
}
