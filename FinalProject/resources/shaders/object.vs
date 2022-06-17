//Vertex shader
#version 330 core
layout (location = 0) in vec3 position_modelSpace;
layout (location = 1) in vec3 normal_modelSpace;
layout (location = 2) in vec3 texCoords_modelspace;

uniform mat4 model;
uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform vec3 lightPosition_viewSpace;

out vec4 positionVector_viewSpace;
out vec3 normalVector_viewSpace;
out vec3 lightVector_viewSpace;
out vec2 texCoords;
out vec3 positionVector_worldSpace;

void main()
{
    //Calculate position vector in view space
    positionVector_viewSpace = mvMatrix * vec4(position_modelSpace, 1.0);
    // normal vector in view space
    normalVector_viewSpace = normalize(transpose(inverse(mat3(model))) * normal_modelSpace);
    // light direction in view space (you can calculate light position in view space in CPU)
    lightVector_viewSpace = normalize(lightPosition_viewSpace.xyz - positionVector_viewSpace.xyz);
    
    texCoords = texCoords_modelspace.xy;
    
    positionVector_worldSpace = vec3(model * vec4(position_modelSpace, 1.0));
    
    gl_Position = mvpMatrix * vec4(position_modelSpace, 1.0);
}
