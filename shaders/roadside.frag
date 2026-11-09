#version 330 core
in vec3 vColor;
in vec3 vPosition;
uniform vec3 uCameraPosition;
uniform float uAlternatePalette;
out vec4 fragColor;
void main(){
    vec3 color=mix(vColor,vColor.bgr,uAlternatePalette);
    float visibility=1.0-smoothstep(65.0,240.0,length(vPosition-uCameraPosition));
    fragColor=vec4(mix(vec3(0.018,0.012,0.040),color,visibility),1);
}
