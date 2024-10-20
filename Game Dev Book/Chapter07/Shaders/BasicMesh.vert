// request GLSL 3.3
#version 330

// uniforms for world transform and view-proj
uniform mat4 uWorldTransform;  // an "uniform" is a global variable that stays the same between numerous invocations of the shader program
uniform mat4 uViewProj;

// any vertex attributes go here
// we must specify which attribute slot corresponds to which in variable since we now have multiple vertex attributes
layout(location=0) in vec3 inPosition;  // the "location" value corresponds to the slot number in the glVertexAttribPointer call
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTexCoord;

// declare a global out variable to pass texture coord data from vertex shader to fragment shader (so frag shader can determine the color at each pixel)
out vec2 fragTexCoord;

void main() {
    // convert the 3D inPosition into homogeneous coordinates
    vec4 pos = vec4(inPosition, 1.0);  // this position is in object space
    // transform pos into world space by multiplying it by the world transform matrix
    vec4 worldPos = pos * uWorldTransform;
    // transform worldPos into clip space by multiplying it by the view-projection matrix
    gl_Position = worldPos * uViewProj;

    // copy tex coord directly from input to output variable, to pass along the texture coord to frag shader
    fragTexCoord = inTexCoord;
}