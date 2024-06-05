#version 430 core
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

uniform sampler2D u_dy;
uniform int u_N;

layout (binding = 0, rgba32f) writeonly uniform image2D normalMap;

void main()
{
	ivec2 x = ivec2(gl_GlobalInvocationID.xy);
	vec2 texelCoord = gl_GlobalInvocationID.xy / float(u_N);
	float texelSize = 1.0 / float(u_N);

	// Edge detection with Sobel operator
	float n0 = texture(u_dy, texelCoord + vec2(-texelSize, -texelSize)).r; // top left
	float n1 = texture(u_dy, texelCoord + vec2(0, -texelSize)).r;          // top middle
	float n2 = texture(u_dy, texelCoord + vec2(texelSize, -texelSize)).r;  // top right
	float n3 = texture(u_dy, texelCoord + vec2(-texelSize, 0)).r;          // left
	float n4 = texture(u_dy, texelCoord + vec2(texelSize, 0)).r;           // right
	float n5 = texture(u_dy, texelCoord + vec2(-texelSize, texelSize)).r;  // bottom left
	float n6 = texture(u_dy, texelCoord + vec2(0, texelSize)).r;           // bottom middle
	float n7 = texture(u_dy, texelCoord + vec2(texelSize, texelSize)).r;   // bottom right

	vec3 normal;
	// [  1  2  1 ]
	// [  0  0  0 ]
	// [ -1 -2 -1 ]
	normal.z = (n0 + 2.0 * n1 + n2 - n5 - 2.0 * n6 - n7);
	// [ 1 0 -1 ]
	// [ 2 0 -2 ]
	// [ 1 0 -1 ]
	normal.x = (n0 + 2.0 * n3 + n5 - n2 - 2.0 * n4 - n7);
	normal.y = 1.0;

	imageStore(normalMap, x, vec4(normalize(normal), 1.0));

}