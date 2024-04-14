#version 430 core
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

const float M_PI = 3.1415926535897932384626433832795;

uniform int u_pingpong;
uniform int u_isVertical;
uniform int u_stage;

layout (binding = 0, rgba32f) readonly uniform image2D twiddleFactors;
layout (binding = 1, rgba32f) uniform image2D ping;
layout (binding = 2, rgba32f) uniform image2D pong;

vec2 complex_mul(vec2 c0, vec2 c1)
{
    return vec2(c0.x * c1.x - c0.y * c1.y, c0.x * c1.y + c0.y * c1.x);
}

vec2 complex_conjugate(vec2 c)
{
    return vec2(c.x, -c.y);
}

void horizontal()
{
    vec2 h;
    ivec2 x = ivec2(gl_GlobalInvocationID.xy);
    vec4 data = imageLoad(twiddleFactors, ivec2(u_stage, x.x)).rgba;
    vec2 w = vec2(data.xy);

    if(u_pingpong == 0)
    {
        vec2 p = imageLoad(ping, ivec2(data.z, x.y)).rg;
        vec2 q = imageLoad(ping, ivec2(data.w, x.y)).rg;

        h = p + complex_mul(w, q);

        imageStore(pong, x, vec4(h, 0, 1));
    }
    else
    {
        vec2 p = imageLoad(pong, ivec2(data.z, x.y)).rg;
        vec2 q = imageLoad(pong, ivec2(data.w, x.y)).rg;

        h = p + complex_mul(w, q);

        imageStore(ping, x, vec4(h, 0, 1));
    }
}

void vertical()
{
    vec2 h;
    ivec2 x = ivec2(gl_GlobalInvocationID.xy);
    vec4 data = imageLoad(twiddleFactors, ivec2(u_stage, x.y)).rgba;
    vec2 w = vec2(data.xy);

    if(u_pingpong == 0)
    {
        vec2 p = imageLoad(ping, ivec2(x.x, data.z)).rg;
        vec2 q = imageLoad(ping, ivec2(x.x, data.w)).rg;

        h = p + complex_mul(w, q);

        imageStore(pong, x, vec4(h, 0, 1));
    }
    else
    {
        vec2 p = imageLoad(pong, ivec2(x.x, data.z)).rg;
        vec2 q = imageLoad(pong, ivec2(x.x, data.w)).rg;

        h = p + complex_mul(w, q);

        imageStore(ping, x, vec4(h, 0, 1));
    }
}

void main()
{
    if(u_isVertical == 0)
    {
        horizontal();
    }
    else
    {
        vertical();
    }
}