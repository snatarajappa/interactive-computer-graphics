// Plane vertex shader
#version 330 core
layout(location = 0) in vec3 position_modelSpace;

vec3 normal_modelSpace = vec3(0.0f, 1.0f, 0.0f);

uniform mat4 model;
uniform mat4 mvMatrix;
uniform mat4 planeMVPMatrix;
uniform mat4 normalMVMatrix;
uniform vec3 lightPosition_viewSpace;

out vec3 positionVector_worldSpace;
out vec3 normalVector_worldSpace;

out vec4 positionVector_viewSpace;
out vec3 normalVector_viewSpace;
out vec3 lightVector_viewSpace;

void main(){
    //Calculate position vector in view space
    positionVector_viewSpace = mvMatrix * vec4(0.05*position_modelSpace, 1.0);
    // normal vector in view space
    normalVector_viewSpace = normalize(vec3(normalMVMatrix * vec4(normal_modelSpace,1)));
    // light direction in view space (you can calculate light position in view space in CPU)
    lightVector_viewSpace = normalize(lightPosition_viewSpace.xyz - positionVector_viewSpace.xyz);
    
    //Calculate position vector in view space
    positionVector_worldSpace = vec3(model * vec4(position_modelSpace, 1.0));
    // normal vector in view space
    normalVector_worldSpace = mat3(transpose(inverse(model))) * normal_modelSpace;
    gl_Position =  planeMVPMatrix * vec4(position_modelSpace,1);
}
