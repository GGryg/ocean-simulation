#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 texCoord;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);

    texCoord = aTex;
}