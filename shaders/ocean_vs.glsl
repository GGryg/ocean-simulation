#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

uniform sampler2D u_dx;
uniform sampler2D u_dy;
uniform sampler2D u_dz;
uniform sampler2D u_normalMap;

uniform float u_displacement;
uniform float u_choppiness;

out vec2 texCoord;
out vec3 fragPosition;
out vec3 normals;

void main()
{

    vec3 pos = aPos;
    pos.x -= texture(u_dx, aTex).r * u_choppiness;
    pos.y += texture(u_dy, aTex).r * u_displacement;
    pos.z -= texture(u_dz, aTex).r * u_choppiness;
    gl_Position = u_proj * u_view * u_model * vec4(pos, 1.0);

    texCoord = aTex;
    fragPosition = vec3(u_model * vec4(pos, 1.0));
    normals = texture(u_normalMap, aTex).xyz;
}