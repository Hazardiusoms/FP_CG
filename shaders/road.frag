#version 330 core
in vec2 vUv;
in vec3 vWorldPosition;
uniform float uOffset;
uniform float uAlternatePalette;
uniform float uRoadHalfWidth;
uniform vec3 uCameraPosition;
out vec4 fragColor;
void main() {
    vec2 uv=vUv-vec2(0.0,uOffset);
    vec2 footprint=max(fwidth(uv),vec2(0.0001));
    vec2 distanceToLine=abs(fract(uv+0.5)-0.5);
    vec2 lines=1.0-smoothstep(vec2(0.018),vec2(0.018)+footprint,distanceToLine);
    // Fade subpixel repetition before it turns into a solid bright horizon.
    float grid=max(lines.x,lines.y)*(1.0-smoothstep(0.15,0.8,max(footprint.x,footprint.y)));
    float x=abs(vWorldPosition.x);
    float edgeFootprint=max(fwidth(x),0.005);
    float edge=1.0-smoothstep(0.035,0.035+edgeFootprint,abs(x-uRoadHalfWidth));
    float inside=1.0-smoothstep(uRoadHalfWidth,uRoadHalfWidth+edgeFootprint,x);
    vec3 ground=mix(vec3(0.027,0.009,0.055),vec3(0.012,0.020,0.042),inside);
    vec3 gridColor=mix(vec3(0.54,0.04,0.70),vec3(0.06,0.52,0.65),inside);
    gridColor=mix(gridColor,vec3(0.9,0.36,0.06),uAlternatePalette);
    vec3 color=ground+grid*gridColor+edge*vec3(0.95,0.10,0.46);
    float distanceToCamera=length(vWorldPosition-uCameraPosition);
    float visibility=1.0-smoothstep(45.0,200.0,distanceToCamera);
    color=mix(vec3(0.018,0.012,0.040),color,visibility);
    fragColor=vec4(color,1.0);
}
