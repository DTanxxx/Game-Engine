/* request GLSL 3.3 */
#version 330

/* tex coord input from vertex shader */ 
in vec2 fragTexCoord;  /* corresponds to the out variable in vertex shader */
/* normal (in world space) */
in vec3 fragNormal;
/* position (in world space) */
in vec3 fragWorldPos;

/* this is output color to the color buffer */
out vec4 outColor;  /* RGBA */

/* create a uniform for texture sampler that can get the color from a texture given a texture coordinate */
uniform sampler2D uTexture;

/* create a struct for directional light */
struct DirectionalLight {
    // direction of light
    vec3 mDirection;
    // diffuse color
    vec3 mDiffuseColor;
    // specular color
    vec3 mSpecColor;
};

/* uniforms for lighting */
/* camera position (in world space) */
uniform vec3 uCameraPos;
/* ambient light level */
uniform vec3 uAmbientLight;
/* specular power for this surface */
uniform float uSpecPower;

/* directional light */
uniform DirectionalLight uDirLight;

void main() {
    /* surface normal */
    vec3 N = normalize(fragNormal);
    /* vector from surface to light (negation of directional light's direction) */
    vec3 L = normalize(-uDirLight.mDirection);
    /* vector from surface to camera */
    vec3 V = normalize(uCameraPos - fragWorldPos);
    /* reflection of -L about N */
    vec3 R = normalize(reflect(-L, N));

    /* compute phong reflection */
    vec3 phong = uAmbientLight;
    float NdotL = dot(N, L);
    if (NdotL > 0) {
        vec3 diffuse = uDirLight.mDiffuseColor * NdotL;
        vec3 specular = uDirLight.mSpecColor * pow(max(0.0, dot(R, V)), uSpecPower);
        phong += diffuse + specular;
    }

    /* final color is texture color times phong light (alpha = 1) */
    outColor = texture(uTexture, fragTexCoord) * vec4(phong, 1.0f);
}