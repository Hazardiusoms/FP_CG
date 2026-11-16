#version 330 core
// Per-fragment Blinn-Phong with ambient, diffuse, specular and emissive terms, evaluated in
// world space. Light 0 is directional; the rest are point lights with distance attenuation
// (the moving accent light and the roadside lamp posts).
const int LIGHT_COUNT = 10;   // must equal lightCount in LightingModel.hpp
struct Light {
    vec3 vector;      // directional: toward the light; point: world position
    float isPoint;    // 0 or 1
    vec3 color;
    float linear;
    float quadratic;
};
in vec3 vWorldPosition;
in vec3 vNormal;
uniform vec3 uCameraPosition;
uniform vec3 uAmbient;
uniform Light uLights[LIGHT_COUNT];
uniform vec3 uBaseColor;
uniform vec3 uEmissive;
uniform float uSpecular;
uniform float uShininess;
out vec4 fragColor;
void main() {
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(uCameraPosition - vWorldPosition);
    vec3 color = uBaseColor * uAmbient + uEmissive;
    for (int i = 0; i < LIGHT_COUNT; ++i) {
        vec3 toLight = uLights[i].vector;
        float attenuation = 1.0;
        if (uLights[i].isPoint > 0.5) {
            vec3 offset = uLights[i].vector - vWorldPosition;
            float distance = length(offset);
            toLight = offset / max(distance, 1e-4);
            attenuation = 1.0 / (1.0 + uLights[i].linear * distance + uLights[i].quadratic * distance * distance);
        }
        float diffuse = max(dot(normal, toLight), 0.0);
        // Blinn-Phong half vector; no highlight on surfaces facing away from the light.
        vec3 halfway = normalize(toLight + viewDir);
        float highlight = diffuse > 0.0 ? pow(max(dot(normal, halfway), 0.0), uShininess) : 0.0;
        color += attenuation * uLights[i].color * (uBaseColor * diffuse + vec3(uSpecular * highlight));
    }
    fragColor = vec4(color, 1.0);
}
