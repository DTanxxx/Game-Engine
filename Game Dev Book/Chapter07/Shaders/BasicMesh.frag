// request GLSL 3.3
#version 330

// create a uniform for texture sampler that can get the color from a texture given a texture coordinate
uniform sampler2D uTexture;

// tex coord input from vertex shader
in vec2 fragTexCoord;  // corresponds to the out variable in vertex shader

// this is output color to the color buffer
out vec4 outColor;  // RGBA

void main() {
    // sample color from the texture, using the texture coordinates received from the vertex shader
    outColor = texture(uTexture, fragTexCoord);
}