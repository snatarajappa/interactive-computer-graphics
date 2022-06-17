//Vertex shader for cube
#version 330 core
layout (location = 0) in vec3 position;

out vec3 textureDir;

uniform mat4 mvpMatrix;

void main()
{
    gl_Position = mvpMatrix * vec4(position, 1.0);
    textureDir = vec3(position);
}
