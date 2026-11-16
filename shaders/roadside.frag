#version 330 core
in vec3 vColor;
in vec3 vPosition;
uniform vec3 uCameraPosition;
uniform float uAlternatePalette;
uniform vec3 uLampColor;   // lamp-post light colour (palette aware, set by the CPU)
uniform float uTravel;     // road travel: lamp posts sit at multiples of 12 m of (z - travel)
uniform float uGlow;       // 1 for the lamp heads themselves
out vec4 fragColor;
void main(){
    vec3 color=mix(vColor,vColor.bgr,uAlternatePalette);
    // Light from the nearest lamp head falls on rails and posts around it.
    float dz=abs(fract((vPosition.z-uTravel)/12.0+0.5)-0.5)*12.0;
    float dx=abs(abs(vPosition.x)-4.0);
    float dy=vPosition.y-2.94;
    color+=uLampColor*exp(-(dz*dz+0.8*dx*dx+0.6*dy*dy)*0.2)*0.4;
    // A burning lamp head: close to white in the lamp colour, brighter than 1 so bloom picks it up.
    color=mix(color,mix(uLampColor,vec3(1.0),0.5)*1.8,uGlow);
    float visibility=1.0-smoothstep(65.0,240.0,length(vPosition-uCameraPosition));
    fragColor=vec4(mix(vec3(0.018,0.012,0.040),color,visibility),1);
}
