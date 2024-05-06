#version 430 core
out vec4 FragColor;

const float M_PI = 3.1415926535897932384626433832795;

in vec2 texCoord;
in vec3 fragPosition;
in vec3 normals;

uniform sampler2D u_normalMap;
uniform samplerCube u_skybox;

uniform vec3 u_viewPosition;
uniform int u_wireframeMode;

uniform float u_roughness;
uniform vec3 u_sunDirection;
uniform vec3 u_sunIrradiance;
uniform vec3 u_scatterColor;
uniform vec3 u_bubbleColor;
uniform float u_bubbleDensity;
uniform float u_wavePeakScatterStrength;
uniform float u_scatterStrength;
uniform float u_scatterShadowStrength;
uniform float u_waveHeight;
uniform float u_envLightStrength;

float smithMaskingBeckmann(vec3 H, vec3 S, float roughness)
{
    float HdotS = max(dot(H, S), 0.0001);
    float a = HdotS / (roughness * sqrt(1.0 - HdotS * HdotS));
    float a2 = a * a;

    return a < 1.6 ? (1.0 - 1.259 * a + 0.396 * a2) / (3.535 * a + 2.181 * a2) : 0.0;
}

float beckmann(vec3 N, vec3 H, float roughness)
{
    float NdotH = max(dot(N, H), 0.0001);
    float p22 = exp((NdotH * NdotH - 1.0) / (roughness * roughness * NdotH * NdotH));

    return p22 / (M_PI * roughness * roughness * NdotH * NdotH * NdotH * NdotH);
}

// Based on "Wakes, Explosions and Lighting: Interactive Water Simulation in Atlas"
void main()
{
    if(u_wireframeMode == 1)
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    vec3 N = normalize(normals);
    vec3 V = normalize(u_viewPosition - fragPosition);
    if (dot(N, V) < 0.0) N = -N;

    vec3 L = -normalize(u_sunDirection);
    vec3 halfwayDir = normalize(L + V);

    float a = u_roughness;
    float viewMask = smithMaskingBeckmann(halfwayDir, V, a);
    float lightMask = smithMaskingBeckmann(halfwayDir, L, a);

    float G = 1.0 / (1.0 + viewMask + lightMask);
    float eta = 1.33;
    float R = ((eta - 1.0) * (eta - 1)) / ((eta + 1.0) * (eta + 1.0));
    float thetaV = acos(V.y);

    float numerator = pow(1.0 - dot(N, V), 5.0 * exp(-2.69 * a));
    float F = R + (1 + R) * numerator / (1.0 + 22.7 * pow(a, 1.5));
    F = clamp(F, 0.0, 1.0);

    vec3 specular = u_sunIrradiance * F * G * beckmann(N, halfwayDir, a);
    specular /= 4.0 * max(0.0001, dot(N, L));
    specular *= max(0.0001, dot(N, L));

    float H = max(0.0, fragPosition.y) * u_waveHeight;

    float k1 = u_wavePeakScatterStrength * H * pow(max(0.0001, dot(L, -V)), 4.0) * pow(0.5 - 0.5 * dot(L, N), 3.0);
    float k2 = u_scatterStrength * pow(max(0.0001, dot(V, N)), 2.0);
    float k3 = u_scatterShadowStrength * dot(N, L);
    float k4 = u_bubbleDensity;

    vec3 scatter = (k1 + k2) * u_scatterColor * u_sunIrradiance * (1.0/(1.0 + lightMask));
    scatter += k3 * u_scatterColor * u_sunIrradiance + k4 * u_bubbleColor * u_sunIrradiance;

    vec3 envReflection = texture(u_skybox, reflect(-V, N)).rgb;
    envReflection *= u_envLightStrength;

    vec3 color = (1.0 - F) * scatter + specular + F * envReflection;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/1.6));
    FragColor = vec4(color, 1.0);
}