//control shader
#version 410 core
layout(vertices = 4) out;

in vec3 position_vs[];
in vec2 texCoords[];

out vec3 position_cs[];
out vec2 texCoords_cs[];

uniform float tessellationLevel;

void main()
{
    position_cs[gl_InvocationID] = position_vs[gl_InvocationID];
    texCoords_cs[gl_InvocationID] = texCoords[gl_InvocationID];
    gl_TessLevelOuter[0] = 8.0 * tessellationLevel;
    gl_TessLevelOuter[1] = 8.0 * tessellationLevel;
    gl_TessLevelOuter[2] = 8.0 * tessellationLevel;
    gl_TessLevelOuter[3] = 8.0 * tessellationLevel;
    
    gl_TessLevelInner[0] = 8.0 * tessellationLevel;
    gl_TessLevelInner[1] = 8.0 * tessellationLevel;
    
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
