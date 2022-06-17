//Fragment shader
#version 330 core
out vec4 color;
in vec3 position_worldSpace;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v);
void main()
{
    vec2 texCoords = sampleSphericalMap(normalize(position_worldSpace));
    color = vec4(texture(equirectangularMap, texCoords).rgb, 1.0);
}

vec2 sampleSphericalMap(vec3 position)
{
    vec2 texCoords = vec2(atan(position.z, position.x), asin(position.y));
    texCoords *= invAtan;
    texCoords += 0.5;
    return texCoords;
}
