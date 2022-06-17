//vertex shader
#version 330 core
layout(location = 0) in vec3 position_modelSpace;

uniform mat4 mvpMatrix;
uniform mat4 model;

out vec2 texCoords;
out vec3 position_vs;

void main()
{
    texCoords = (position_modelSpace.xy+vec2(1,1))/2.0;
    position_vs = vec3(model * vec4(position_modelSpace, 1.0));
    gl_Position = mvpMatrix * vec4(position_modelSpace, 1.0);
}
