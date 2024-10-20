// request GLSL 3.3
#version 330

// any vertex attributes go here
// for now, just a position
in vec3 inPosition;  // the "in" keyword marks this variable as an input to the vertex shader

void main() {
    // directly pass along inPosition to gl_Position
    gl_Position = vec4(inPosition, 1.0);  // gl_Position is vec4, so we need to convert from vec3 to vec4
}