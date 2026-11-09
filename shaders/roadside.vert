#version 330 core
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aColor;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uShift;
out vec3 vColor;
out vec3 vPosition;
void main(){
    vPosition=aPosition+vec3(0,0,uShift);
    vColor=aColor;
    gl_Position=uProjection*uView*vec4(vPosition,1);
}
