#version 430 core
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

const float M_PI = 3.1415926535897932384626433832795;
const float M_GRAVITY = 9.81;
const float MIN_NOISE_CLAMP = 0.01;
const float MAX_NOISE_CLAMP = 1.0;
const float MIN_HEIGHT_CLAMP = -2000.0;
const float MAX_HEIGHT_CLAMP = 2000.0;

uniform float u_amplitude;
uniform float u_windSpeed;
uniform vec2 u_WindDirection;
uniform int u_N;
uniform float u_L;
uniform float u_supressorFactor;

layout (binding = 0, rgba32f) writeonly uniform image2D tilde_h_0_k;
layout (binding = 1, rgba32f) writeonly uniform image2D tilde_h_0_minusk;

layout (binding = 2) uniform sampler2D noise0;
layout (binding = 3) uniform sampler2D noise1;
layout (binding = 4) uniform sampler2D noise2;
layout (binding = 5) uniform sampler2D noise3;

// Box-Muller transform
vec4 gaussian_random_number(vec2 texelCoord)
{
    float u0 = 2.0 * M_PI * clamp(texture(noise0, texelCoord).r, MIN_NOISE_CLAMP, MAX_NOISE_CLAMP);
    float v0 = sqrt(-2.0* log(clamp(texture(noise1, texelCoord).r, MIN_NOISE_CLAMP, MAX_NOISE_CLAMP)));
    float u1 = 2.0 * M_PI * clamp(texture(noise2, texelCoord).r, MIN_NOISE_CLAMP, MAX_NOISE_CLAMP);
    float v1 = sqrt(-2.0 * log(clamp(texture(noise3, texelCoord).r, MIN_NOISE_CLAMP, MAX_NOISE_CLAMP)));

    return vec4(v0 * cos(u0), v0 * sin(u0), v1 * cos(u1), v1 * sin(u1));
}

// Phillips spectrum
float phillipsSecptrum(vec2 k, float k_magnitute, float L_phillips)
{
    float k_magnitute_2 = k_magnitute * k_magnitute;
    float k_magnitute_4 = k_magnitute_2 * k_magnitute_2;
    float KdotW = dot(normalize(k), normalize(u_WindDirection));
    // Ph(k) = A * (exp(-1/(k*L)^2) / k^4) * |khat dot what|^2
    float Phk = u_amplitude * ((exp(-1.0 / (k_magnitute_2 * L_phillips * L_phillips))) / k_magnitute_4) * pow(abs(KdotW), 8);
    if(KdotW < 0.0)
    {
        Phk *= 0.07;
    }
    return Phk * exp(-k_magnitute_2 * u_supressorFactor * u_supressorFactor);
}

// Based on Simulation Ocean Water by Jerry Tessendorf
void main()
{
    vec2 x = vec2(gl_GlobalInvocationID.xy) - u_N / 2.0;
    vec2 k = vec2(2.0 * M_PI * x.x / float(u_L), 2.0 * M_PI * x.y / float(u_L));

    // L = V^2 / g
    float L_phillips = u_windSpeed * u_windSpeed / M_GRAVITY;

    float k_magnitute = length(k);
    if(k_magnitute < 0.000001)
    {
        k_magnitute = 0.000001;
    }

    // sqrt(Ph(k)) / sqrt(2)
    float h0k = clamp(sqrt(phillipsSecptrum(k, k_magnitute, L_phillips)) / sqrt(2.0), MIN_HEIGHT_CLAMP, MAX_HEIGHT_CLAMP);
    // sqrt(Ph(-k)) / sqrt(2)
    float h0minusk = clamp(sqrt(phillipsSecptrum(-k, k_magnitute, L_phillips)) / sqrt(2.0), MIN_HEIGHT_CLAMP, MAX_HEIGHT_CLAMP);

    vec4 random = gaussian_random_number(vec2(gl_GlobalInvocationID.xy) / float(u_N));

    imageStore(tilde_h_0_k, ivec2(gl_GlobalInvocationID.xy), vec4(random.xy * h0k, 0.0, 1.0));
    imageStore(tilde_h_0_minusk, ivec2(gl_GlobalInvocationID.xy), vec4(random.zw * h0minusk, 0.0, 1.0));
}