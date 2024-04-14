#version 430 core
layout (local_size_x = 1, local_size_y = 32, local_size_z = 1) in;

const float M_PI = 3.1415926535897932384626433832795;

layout (binding = 0, rgba32f) writeonly uniform image2D twiddleFactors;

layout (std430, binding = 1) buffer indices
{
    int j[];
} bitReversedIndices;

uniform int u_N;

void main()
{
    vec2 x = gl_GlobalInvocationID.xy;
    int stage = int(x.x);
    int indexInStage = int(x.y);
    float k = mod(indexInStage * (u_N / pow(2, stage+1)), u_N);
    vec2 twiddle = vec2(cos(2.0 * M_PI * k / float(u_N)), sin(2.0 * M_PI * k / float(u_N)));

    int butterflySpan = int(pow(2, stage));
    int butterflyWing = int(mod(indexInStage, pow(2, stage + 1))) < butterflySpan ? 1 : 0;

    int topIndex = butterflyWing == 1 ? indexInStage : indexInStage - 1;
    int bottomIndex = butterflyWing == 1 ? indexInStage + 1 : indexInStage;

    int topBitReversedIndex = bitReversedIndices.j[topIndex];
    int bottomBitReversedIndex = bitReversedIndices.j[bottomIndex];

    if (stage == 0)
    {
        imageStore(twiddleFactors, ivec2(x), vec4(twiddle.xy, topBitReversedIndex, bottomBitReversedIndex));
    } 
    else
    {
        if (butterflyWing == 1)
        {
            imageStore(twiddleFactors, ivec2(x), vec4(twiddle.xy, indexInStage, indexInStage + butterflySpan));
        }
        else
        {
            imageStore(twiddleFactors, ivec2(x), vec4(twiddle.xy, indexInStage - butterflySpan, indexInStage));
        }
    }
}