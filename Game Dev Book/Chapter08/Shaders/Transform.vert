// request GLSL 3.3
#version 330

// uniforms for world transform and view-proj
uniform mat4 uWorldTransform;  // an "uniform" is a global variable that stays the same between numerous invocations of the shader program
uniform mat4 uViewProj;

// any vertex attributes go here
in vec3 inPosition;  // the "in" keyword marks this variable as an input to the vertex shader

void main() {
    // convert the 3D inPosition into homogeneous coordinates
    vec4 pos = vec4(inPosition, 1.0);  // this position is in object space
    // transform pos into world space by multiplying it by the world transform matrix
    vec4 worldPos = pos * uWorldTransform;
    // transform worldPos into clip space by multiplying it by the view-projection matrix
    gl_Position = worldPos * uViewProj;
}