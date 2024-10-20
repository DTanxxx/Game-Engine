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

/* create a struct for point light */
struct PointLight {
	vec3 mPosition;
	vec3 mDiffuseColor;
	vec3 mSpecColor;
	float mSpecPower;
	float mRadius;
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

/* point lights */
uniform PointLight uPointLights[4];

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

    /* include point lights */
    for (int i = 0; i < uPointLights.length(); ++i) {
        /* check if this pixel is within this light's radius */
        if (distance(fragWorldPos, uPointLights[i].mPosition) <= uPointLights[i].mRadius) {
            vec3 diffuse = uPointLights[i].mDiffuseColor;
            vec3 specular = uPointLights[i].mSpecColor * pow(dot(N, V), uSpecPower);
            phong += diffuse + specular;
        }
    }

    /* clamp phong values */
    phong = clamp(phong, 0.0, 1.0);

    /* final color is texture color times phong light (alpha = 1) */
    outColor = texture(uTexture, fragTexCoord) * vec4(phong, 1.0f);
}