//Vertex shader
#version 330 core
layout (location = 0) in vec3 position_worldSpace;
layout (location = 1) in vec3 normal_worldSpace;
layout (location = 2) in vec3 textureVertices_modelspace;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 normalMVMatrix;
uniform vec3 lightPosition_viewSpace;

out vec4 positionVector_viewSpace;
out vec3 normalVector_viewSpace;
out vec3 lightVector_viewSpace;
out vec2 textureCoordinates;

void main()
{
    //Calculate position vector in view space
    positionVector_viewSpace = mvMatrix * vec4(0.05 *position_worldSpace.x, 0.05 *position_worldSpace.y, 0.05 *position_worldSpace.z, 1.0);
    // normal vector in view space
    normalVector_viewSpace = normalize(mat3(normalMVMatrix) * normal_worldSpace);
    // light direction in view space (you can calculate light position in view space in CPU)
    lightVector_viewSpace = normalize(lightPosition_viewSpace.xyz - positionVector_viewSpace.xyz);
    //Calculate texture co-ordinates
    textureCoordinates = vec2(textureVertices_modelspace.x, textureVertices_modelspace.y);
    gl_Position = mvpMatrix * vec4(0.05 *position_worldSpace.x, 0.05 *position_worldSpace.y, 0.05 *position_worldSpace.z, 1.0);
}

