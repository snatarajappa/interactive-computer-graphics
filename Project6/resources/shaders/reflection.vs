//Vertex shader
#version 330 core
layout (location = 0) in vec3 position_modelSpace;
layout (location = 1) in vec3 normal_modelSpace;

uniform mat4 mvMatrix;
uniform mat4 model;
uniform mat4 mvpMatrix;
uniform mat4 normalMVMatrix;
uniform vec3 lightPosition_viewSpace;

out vec4 positionVector_viewSpace;
out vec3 normalVector_viewSpace;
out vec3 lightVector_viewSpace;
out vec3 cameraPosition_worldSpace;

//out vec3 positionVector_worldSpace;
out vec3 normalVector_worldSpace;

void main()
{
    //Calculate position vector in view space
    positionVector_viewSpace = mvMatrix * vec4(0.05*position_modelSpace, 1.0);
    // normal vector in view space
    normalVector_viewSpace = normalize(vec3(normalMVMatrix * vec4(normal_modelSpace,1)));
    // light direction in view space (you can calculate light position in view space in CPU)
    lightVector_viewSpace = normalize(lightPosition_viewSpace.xyz - positionVector_viewSpace.xyz);
    
    //Environment mapping
    //    positionVector_worldSpace = vec3(model * vec4(position_modelSpace, 1.0));
    normalVector_worldSpace = vec3(transpose(inverse(model)) * vec4(normal_modelSpace.x,normal_modelSpace.y,normal_modelSpace.z,1));
    cameraPosition_worldSpace = vec3(inverse(mvMatrix)*vec4(0,0,-1,0));
    
    gl_Position = mvpMatrix * vec4(0.05 *position_modelSpace, 1.0);
}
