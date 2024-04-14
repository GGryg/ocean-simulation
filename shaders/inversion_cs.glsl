#version 430 core
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

uniform int u_pingpong;
uniform int u_N;

layout (binding = 0, rgba32f) writeonly uniform image2D displacement;
layout (binding = 1, rgba32f) readonly uniform image2D ping;
layout (binding = 2, rgba32f) readonly uniform image2D pong;

void main()
{
    ivec2 x = ivec2(gl_GlobalInvocationID.xy);

    float perms[] = {1.0, -1.0};
    int index = int(mod((int(x.x + x.y)), 2));
    float perm = perms[index];

    if(u_pingpong == 0)
    {
        float h = imageLoad(ping, x).r;
        float data = perm * (h / float(u_N * u_N));
        imageStore(displacement, x, vec4(data, data, data, 1));
    }
    else
    {
        float h = imageLoad(pong, x).r;
        float data = perm * (h / float(u_N * u_N));
        imageStore(displacement, x, vec4(data, data, data, 1));
    }
}

