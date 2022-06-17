//Evaluation shader
#version 410 core
layout(quads, equal_spacing, ccw) in;

in vec3 position_cs[];
in vec2 texCoords_cs[];

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform float displacementLevel;
uniform sampler2D dispMap;
uniform vec3 lightPosition_viewSpace;
uniform mat4 lpMatrix;
uniform mat4 model;

out vec3 lightVector_viewSpace;
out vec4 positionVector_viewSpace;
out vec2 texCoords;
out vec3 lightPos_forBias;
out vec3 FragPos_forBias;
out vec4 fragPosition_lightSpace;
out vec3 normal_forBias;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2, vec2 v3){
    vec2 a = mix(v0, v1, gl_TessCoord.x);
    vec2 b = mix(v3, v2, gl_TessCoord.x);
    return mix(a, b, gl_TessCoord.y);
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2, vec3 v3){
    vec3 a = mix(v0, v1, gl_TessCoord.x);
    vec3 b = mix(v3, v2, gl_TessCoord.x);
    return mix(a, b, gl_TessCoord.y);
}


void main()
{
    texCoords = interpolate2D(texCoords_cs[0], texCoords_cs[1], texCoords_cs[2], texCoords_cs[3]);
    vec3 position_es = interpolate3D(position_cs[0], position_cs[1], position_cs[2], position_cs[3]);
    vec3 displacement = texture(dispMap, vec2(texCoords.x, 1-texCoords.y)).rbg;
    position_es += vec3(0, 0, 1) * displacement * displacementLevel;
    gl_Position = mvpMatrix * vec4(position_es, 1.0);
    
    positionVector_viewSpace = mvMatrix * vec4(position_es, 1.0);
    lightVector_viewSpace = normalize(lightPosition_viewSpace.xyz - positionVector_viewSpace.xyz);
    
    lightPos_forBias = lightPosition_viewSpace;
//    FragPos_forBias = vec3(model * vec4(position_es, 1.0));
    FragPos_forBias = position_es;
//    fragPosition_lightSpace = lpMatrix * vec4(vec3(model* vec4(position_es, 1.0)), 1.0);
    fragPosition_lightSpace = lpMatrix * vec4(position_es, 1.0);
    
    vec3 normal_modelSpace = vec3(0, 0, 1);
    normal_forBias = transpose(inverse(mat3(model))) * normal_modelSpace;
}
