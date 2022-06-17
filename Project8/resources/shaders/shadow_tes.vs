//Evaluation shader
#version 410 core
layout(quads, equal_spacing, ccw) in;

in vec3 position_cs[];
in vec2 texCoords_cs[];

uniform float displacementLevel;
uniform sampler2D dispMap;
uniform mat4 lpMatrix;
uniform mat4 model;

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
    vec2 texCoords = interpolate2D(texCoords_cs[0], texCoords_cs[1], texCoords_cs[2], texCoords_cs[3]);
    vec3 position_es = interpolate3D(position_cs[0], position_cs[1], position_cs[2], position_cs[3]);
    vec3 displacement = texture(dispMap, vec2(texCoords.x, 1-texCoords.y)).rbg;
    position_es += vec3(0, 0, 1) * displacement * displacementLevel;
    gl_Position = lpMatrix * model * vec4(position_es, 1.0);
}

