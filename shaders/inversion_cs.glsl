#version 430 core
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

uniform int u_N;

layout (binding = 0, r32f) uniform image2D displacementX;
layout (binding = 1, r32f) uniform image2D displacementY;
layout (binding = 2, r32f) uniform image2D displacementZ;

vec4 permute(vec4 data, ivec2 id) {
    return data * (1.0 - 2.0 * ((id.x + id.y) % 2));
}

void main()
{
    ivec2 x = ivec2(gl_GlobalInvocationID.xy);

    vec4 dataX = permute(imageLoad(displacementX, x), x);
    imageStore(displacementX, x, dataX);
    vec4 dataY = permute(imageLoad(displacementY, x), x);
    imageStore(displacementY, x, dataY);
    vec4 dataZ = permute(imageLoad(displacementZ, x), x);
    imageStore(displacementZ, x, dataZ);
}

