#version 330 core
out vec4 color;
in vec3 position_worldSpace;

uniform samplerCube environmentMap;

const float PI = 3.1416;

void main()
{
    vec3 N = normalize(position_worldSpace);

    vec3 irradiance = vec3(0.0);
    
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));
       
    float sampleDelta = 0.025;
    float numberOfSamples = 0.0f;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            numberOfSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(numberOfSamples));
    
    color = vec4(irradiance, 1.0);
}
