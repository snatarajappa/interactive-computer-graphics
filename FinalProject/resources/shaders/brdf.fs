#version 330 core
out vec2 color;
in vec2 texCoords;

const float PI = 3.1416;

vec2 computeBRDF(float NdotV, float roughness);
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness);
vec2 hammersley(uint i, uint N);
float vanDerCorput(uint n, uint base);
float geometrySchlickGGX(float dotProduct, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

void main()
{
    color = computeBRDF(texCoords.x, texCoords.y);
}

vec2 computeBRDF(float NdotV, float roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - NdotV*NdotV);
    V.y = 0.0;
    V.z = NdotV;
    
    float A = 0.0;
    float B = 0.0;
    
    vec3 N = vec3(0.0, 0.0, 1.0);
    
    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = hammersley(i, SAMPLE_COUNT);
        vec3 H = importanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);
        
        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);
        
        if(NdotL > 0.0)
        {
            float G = geometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);
            
            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}

vec2 hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), vanDerCorput(i, 2u));
}

float vanDerCorput(uint n, uint base)
{
    float invBase = 1.0 / float(base);
    float result  = 0.0;
    if(n > 0u)
    {
        for(uint i = 0u; i < 32u; i++)
        {
            result += (mod(float(n), 2.0)) * invBase;
            invBase = invBase / 2.0;
            n       = uint(float(n) / 2.0);
        }
    }
    return result;
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
    
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    
    vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float ggx2 = geometrySchlickGGX(max(dot(N, V), 0.0), roughness);
    float ggx1 = geometrySchlickGGX(max(dot(N, L), 0.0), roughness);
    
    return ggx1 * ggx2;
}

float geometrySchlickGGX(float dotProduct, float roughness)
{
    float k = (roughness * roughness) / 2.0;
    return dotProduct / dotProduct * (1.0 - k) + k;
}
