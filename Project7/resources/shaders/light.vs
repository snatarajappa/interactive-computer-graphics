//Vertex shader
#version 330 core
layout (location = 0) in vec3 position_worldSpace;

uniform mat4 mvpMatrix;

void main()
{
    gl_Position = mvpMatrix * vec4(position_worldSpace, 1.0);
}

