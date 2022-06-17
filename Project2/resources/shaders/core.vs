//Vertex shader
#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 mvpMatrix;

void main()
{
    gl_Position = mvpMatrix * vec4(0.05 *vertexPosition_modelspace.x, 0.05 *vertexPosition_modelspace.y, 0.05 *vertexPosition_modelspace.z, 1.0);
}
