#version 430 core
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

const float M_PI = 3.1415926535897932384626433832795;
const float M_GRAVITY = 9.81;

uniform int u_N;
uniform float u_L;
uniform float u_t; // time
uniform float u_amplitude;

layout (binding = 0, rgba32f) readonly uniform image2D tilde_h_0_k;
layout (binding = 1, rgba32f) readonly uniform image2D tilde_h_0_minusk;

layout (binding = 2, rg32f) writeonly uniform image2D tilde_hkt_dx;
layout (binding = 3, rg32f) writeonly uniform image2D tilde_hkt_dy;
layout (binding = 4, rg32f) writeonly uniform image2D tilde_hkt_dz;

vec2 complex_mul(vec2 c0, vec2 c1)
{
	return vec2(c0.x * c1.x - c0.y * c1.y, c0.x * c1.y + c0.y * c1.x);
}

vec2 complex_conjugate(vec2 c)
{
	return vec2(c.x, -c.y);
}

float dispersionCurve(float k_magnitute)
{
	return sqrt(M_GRAVITY * k_magnitute);
}

vec2 eulerFormula(float w)
{
	// exp(ix) = cos(x) + i*sin(x), Euler formula
	// {exp(ix) = cos(x) + i*sin(x)
	// {exp(-ix) = cos(x) - i*sin(x)
	float cosine = cos(w * u_t);
	float sine = sin(w * u_t);

	return vec2(cosine, sine);
}

// Based on Simulation Ocean Water by Jerry Tessendorf
void main()
{
	// -N/2 <= n < N/2
	// m is the same as n
	vec2 n = ivec2(gl_GlobalInvocationID.xy) - u_N / 2.0;
	// k = (kx, kz)
	// kx = 2*PI*n / Lx
	// kz = 2*PI*m / Lz
	vec2 k = vec2(2.0 * M_PI * n.x / u_L, 2.0 * M_PI * n.y / u_L);

	float k_magnitute = length(k);
	if (k_magnitute < 0.000001)
	{
		k_magnitute = 0.000001;
	}

	float w = dispersionCurve(k_magnitute);

	// h0(x, t)
	// x = (x, z), horizontal position
	vec2 h0k = imageLoad(tilde_h_0_k, ivec2(gl_GlobalInvocationID.xy)).rg * u_amplitude;

	vec2 hconj0minusk = complex_conjugate(imageLoad(tilde_h_0_minusk, ivec2(gl_GlobalInvocationID.xy)).rg) * u_amplitude;

	vec2 exp_iwt = eulerFormula(w);
	vec2 exp_minus_iwt = complex_conjugate(exp_iwt);

	// h(k, t) = h0(k)exp(iw(k)*t) + h0(-k)*exp(-iw(k)*t)
	vec2 h_k_t_dy = complex_mul(h0k, exp_iwt) + complex_mul(hconj0minusk, exp_minus_iwt);

	vec2 h_k_t_dx = complex_mul(vec2(0.0, -k.x/k_magnitute), h_k_t_dy);
	vec2 h_k_t_dz = complex_mul(vec2(0.0, -k.y/k_magnitute), h_k_t_dy);

	imageStore(tilde_hkt_dy, ivec2(gl_GlobalInvocationID.xy), vec4(h_k_t_dy, 0.0, 0.0));
	imageStore(tilde_hkt_dx, ivec2(gl_GlobalInvocationID.xy), vec4(h_k_t_dx, 0.0, 0.0));
	imageStore(tilde_hkt_dz, ivec2(gl_GlobalInvocationID.xy), vec4(h_k_t_dz, 0.0, 0.0));
}