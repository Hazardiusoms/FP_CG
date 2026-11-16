#version 330 core
// Additive light cone under a lamp head. The vertex colour is the intensity (bright at the lamp,
// zero at the road); the lamp colour tints it. Facing the camera is brightest, the silhouette
// fades out, so the cone reads as soft light instead of a solid shape.
in vec3 vColor;
in vec3 vPosition;
uniform vec3 uCameraPosition;
uniform vec3 uLampColor;
out vec4 fragColor;
void main(){
    vec3 normal=normalize(cross(dFdx(vPosition),dFdy(vPosition)));
    vec3 toCamera=normalize(uCameraPosition-vPosition);
    float facing=pow(abs(dot(normal,toCamera)),1.5);
    float visibility=1.0-smoothstep(65.0,240.0,length(vPosition-uCameraPosition));
    float intensity=vColor.r*facing*visibility;
    fragColor=vec4(uLampColor*intensity*0.6,1.0);
}
