// request GLSL 3.3
#version 330

// create a uniform for texture sampler that can get the color from a texture given a texture coordinate
uniform sampler2D uTexture;

// tex coord input from vertex shader
in vec2 fragTexCoord;  // corresponds to the out variable in vertex shader
in vec3 fragVertColor;  // Exercise 5.2

// this is output color to the color buffer
out vec4 outColor;  // RGBA

void main() {
    // sample color from the texture, using the texture coordinates received from the vertex shader
    vec4 texColor = texture(uTexture, fragTexCoord);
    float r = (fragVertColor.r + texColor.r) / 2.0f;
    float g = (fragVertColor.g + texColor.g) / 2.0f;
    float b = (fragVertColor.b + texColor.b) / 2.0f;
    outColor = vec4(r, g, b, texColor.a);
}