//Vertex shader
#version 330 core
layout (location = 0) in vec3 position_worldSpace;
layout (location = 1) in vec3 normal_worldSpace;
layout (location = 2) in vec3 textureVertices_modelspace;

uniform mat4 mvMatrix;
uniform mat4 model;
uniform mat4 mvpMatrix;
uniform mat4 normalMVMatrix;
uniform vec3 lightPosition_viewSpace;

uniform mat4 lpMatrix;

out vec4 positionVector_viewSpace;
out vec3 normalVector_viewSpace;
out vec3 lightVector_viewSpace;

out vec4 fragPosition_lightSpace;
out vec3 normal_forBias;
out vec3 lightPos_forBias;
out vec3 FragPos_forBias;

void main()
{
    //Calculate position vector in view space
    positionVector_viewSpace = mvMatrix * vec4(position_worldSpace.x, position_worldSpace.y, position_worldSpace.z, 1.0);
    // normal vector in view space
    normalVector_viewSpace = normalize(mat3(normalMVMatrix) * normal_worldSpace);
    // light direction in view space (you can calculate light position in view space in CPU)
    lightVector_viewSpace = normalize(lightPosition_viewSpace.xyz - positionVector_viewSpace.xyz);
    
    fragPosition_lightSpace = lpMatrix * vec4(vec3(model* vec4(position_worldSpace, 1.0)), 1.0);
    normal_forBias = transpose(inverse(mat3(model))) * normal_worldSpace;
    lightPos_forBias = lightPosition_viewSpace;
    FragPos_forBias = vec3(model * vec4(position_worldSpace, 1.0));
    gl_Position = mvpMatrix * vec4(position_worldSpace, 1.0);
}
