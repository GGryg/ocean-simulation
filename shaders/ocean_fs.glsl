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
uniform vec3 u_sunColor;
uniform vec3 u_scatterColor;
uniform vec3 u_bubbleColor;
uniform float u_bubbleDensity;
uniform float u_wavePeakScatterStrength;
uniform float u_scatterStrength;
uniform float u_scatterShadowStrength;
uniform float u_waveHeight;
uniform float u_envLightStrength;

float smithMaskingBeckmann(vec3 H, vec3 S, float a)
{
	float HdotS = max(dot(H, S), 0.0001);
	float c = HdotS / (a * sqrt(1.0 - pow(HdotS, 2.0)));
	float c2 = pow(c, 2.0);

	return c < 1.6 ? (1.0 - 1.259 * c + 0.396 * c2) / (3.535 * c + 2.181 * c2) : 0.0;
}

float beckmann(vec3 N, vec3 H, float a)
{
	float NdotH = max(dot(N, H), 0.0001);
	float nominator = exp((NdotH * NdotH - 1.0) / (a * pow(NdotH, 2.0)));

	return nominator / (M_PI * a * pow(NdotH, 4.0));
}

float fresnelSchlick(vec3 N, vec3 V, float a)
{
	float eta = 1.0 / 1.333; // refractive index of air / refractive index of water
	float R = ((eta - 1.0) * (eta - 1)) / ((eta + 1.0) * (eta + 1.0));

	float numerator = pow(1.0 - dot(N, V), 5.0 * exp(-2.69 * a));
	float F = R + (1.0 - R) * numerator / (1.0 + 22.7 * pow(a, 1.5));

	return clamp(F, 0.0, 1.0);
}

// Based on "Wakes, Explosions and Lighting: Interactive Water Simulation in Atlas"
void main()
{
	if (u_wireframeMode == 1)
	{
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}
	vec3 N = normalize(normals);
	vec3 V = normalize(u_viewPosition - fragPosition);
	if (dot(N, V) < 0.0) N = -N;

	vec3 L = -normalize(u_sunDirection);
	vec3 H = normalize(L + V);

	float a = pow(u_roughness, 2.0);
	float viewMask = smithMaskingBeckmann(H, V, a);
	float lightMask = smithMaskingBeckmann(H, L, a);

	float G = 1.0 / (1.0 + viewMask + lightMask);
	float F = fresnelSchlick(N, V, a);
	float D = beckmann(N, H, a);

	vec3 specular = u_sunColor * F * G * D;
	specular /= 4.0 * max(0.0001, dot(N, L));
	specular /= max(0.0001, dot(N, V));

	float waveH = max(0.0, fragPosition.y) * u_waveHeight;

	float k1 = u_wavePeakScatterStrength * waveH * pow(max(0.0, dot(L, -V)), 4.0) * pow(0.5 - 0.5 * dot(L, N), 3.0);
	float k2 = u_scatterStrength * pow(max(0.0, dot(V, N)), 2.0);
	vec3 k3 = u_scatterShadowStrength * dot(N, L) * u_scatterColor * u_sunColor;
	vec3 k4 = u_bubbleDensity * u_bubbleColor * u_sunColor;

	vec3 scatter = (k1 + k2) * u_scatterColor * u_sunColor * (1.0 / (1.0 + lightMask));
	scatter += k3 + k4;

	vec3 envReflection = texture(u_skybox, reflect(-V, N)).rgb;
	envReflection *= u_envLightStrength;

	vec3 color = (1.0 - F) * scatter + specular + F * envReflection;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));
	FragColor = vec4(color, 1.0);
}