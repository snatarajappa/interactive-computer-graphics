// Plane vertex shader
#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;

out vec2 UV;

uniform mat4 planeMVPMatrix;

void main(){
    gl_Position =  planeMVPMatrix * vec4(vertexPosition_modelspace,1);
    UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;
}
