#version 330 core
in vec2 vUv;
in vec3 vWorldPosition;
uniform float uOffset;
uniform float uTravel;
uniform float uAlternatePalette;
uniform float uRoadHalfWidth;
uniform vec3 uCameraPosition;
out vec4 fragColor;
float stripe(float distance, float width, float aa) {
    return 1.0-smoothstep(width,width+aa,distance);
}
void main() {
    vec2 uv=vUv-vec2(0,uOffset);
    vec2 footprint=max(fwidth(uv),vec2(0.0001));
    vec2 lines=1.0-smoothstep(vec2(0.012),vec2(0.012)+footprint,abs(fract(uv+0.5)-0.5));
    float grid=max(lines.x,lines.y)*(1.0-smoothstep(0.15,0.8,max(footprint.x,footprint.y)));
    float x=abs(vWorldPosition.x);
    float aa=max(fwidth(x),0.003);
    float inside=1.0-smoothstep(uRoadHalfWidth,uRoadHalfWidth+aa,x);
    vec3 cyan=mix(vec3(0.06,0.8,1),vec3(1,0.52,0.12),uAlternatePalette);
    vec3 pink=mix(vec3(0.8,0.06,0.48),vec3(0.2,0.72,0.9),uAlternatePalette);
    vec3 color=mix(vec3(0.028,0.012,0.055)+grid*pink*0.36,vec3(0.025,0.032,0.054),inside);
    // Six-metre repeating dashes; derivatives soften distant markings.
    float z=vWorldPosition.z-uTravel;
    float phase=abs(fract(z/6.0+0.5)-0.5)*6.0;
    float dash=stripe(phase,1.65,max(fwidth(z),0.01));
    float lanes=stripe(abs(x-uRoadHalfWidth/3.0),0.035,aa)*dash;
    float edge=stripe(abs(x-(uRoadHalfWidth-0.18)),0.035,aa);
    color+=inside*(lanes*vec3(0.55,0.61,0.72)+edge*cyan*0.7);
    float shoulder=stripe(abs(x-uRoadHalfWidth-0.16),0.12,aa);
    color+=shoulder*vec3(0.1,0.07,0.15);
    // Analytic light spill, not screen-space bloom or a claimed reflection.
    color+=cyan*0.13*exp(-abs(x-uRoadHalfWidth)*2.5);
    color+=pink*0.045*exp(-x*0.7);
    float lampZ=abs(fract(z/12.0+0.5)-0.5)*12.0;
    float pool=exp(-lampZ*lampZ*0.35)*exp(-pow(x-3.9,2.0)*0.65);
    color+=inside*pink*pool*0.09;

    float visibility=1.0-smoothstep(60.0,250.0,length(vWorldPosition-uCameraPosition));
    fragColor=vec4(mix(vec3(0.018,0.012,0.040),color,visibility),1);
}
