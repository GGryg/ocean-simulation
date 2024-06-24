#version 430 core
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

uniform int u_N;

layout (binding = 0, r32f) uniform image2D displacementX;
layout (binding = 1, r32f) uniform image2D displacementY;
layout (binding = 2, r32f) uniform image2D displacementZ;

vec4 invert(vec4 data, ivec2 coord) {
	return data * (1.0 - 2.0 * ((coord.x + coord.y) % 2));
}

void main()
{
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	vec4 dataX = invert(imageLoad(displacementX, texelCoord), texelCoord);
	imageStore(displacementX, texelCoord, dataX);
	vec4 dataY = invert(imageLoad(displacementY, texelCoord), texelCoord);
	imageStore(displacementY, texelCoord, dataY);
	vec4 dataZ = invert(imageLoad(displacementZ, texelCoord), texelCoord);
	imageStore(displacementZ, texelCoord, dataZ);
}

