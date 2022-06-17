//Vertex shader
#version 330 core
layout (location = 0) in vec3 position_worldSpace;

uniform mat4 lpMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lpMatrix * model * vec4(position_worldSpace, 1.0);
}


