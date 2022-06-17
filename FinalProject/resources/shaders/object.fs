//Fragment shader
#version 330 core
layout(location = 0) out vec4 color;

in vec4 positionVector_viewSpace;
in vec3 normalVector_viewSpace;
in vec3 lightVector_viewSpace;
in vec2 texCoords;
in vec3 positionVector_worldSpace;

uniform vec3 cameraPosition;
uniform vec3 lightPosition_viewSpace;

// material parameters
uniform vec3  albedoUnifrom;
uniform float metallicUnifrom;
uniform float roughnessUnifrom;
uniform float aoUnifrom;

uniform float maxMipLevels;


uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D normalMap;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap1;
uniform samplerCube prefilterMap2;
uniform samplerCube prefilterMap3;
uniform samplerCube prefilterMap4;
uniform samplerCube prefilterMap5;
uniform sampler2D brdfTexture;

uniform bool enablePBR;
uniform bool enableTexture;
uniform bool enableDiffuseIrradiance;
uniform bool enableSpecularIrradiance;

const float PI = 3.1416;

float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickWithRoughness(float cosTheta, vec3 F0, float roughness);

void main()
{
    vec3 normal = normalVector_viewSpace;
    //View direction is opposit of direction from the camera normalized
    vec3 viewDirection_viewSpace = normalize(cameraPosition - positionVector_worldSpace.xyz);
    //Add view direction + light direction and normalize
    vec3 halfVector = normalize(lightVector_viewSpace + viewDirection_viewSpace);
    
    vec3 reflectionDirection = reflect(-viewDirection_viewSpace, normal);
    
    vec3 vColor;
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    
    if(enablePBR){
        if(enableTexture){
            albedo     = pow(texture(albedoMap, texCoords).rgb, vec3(2.2));
            metallic  = texture(metallicMap, texCoords).r;
            roughness = texture(roughnessMap, texCoords).r;
            ao        = texture(aoMap, texCoords).r;
        }else{
            albedo     = albedoUnifrom;
            metallic  = metallicUnifrom;
            roughness = roughnessUnifrom;
            ao        = aoUnifrom;
        }
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);
        vec3 I = vec3(255,255,255);
        float distance    = length(lightPosition_viewSpace - positionVector_worldSpace.xyz);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = I * attenuation;
        
        // cook-torrance brdf
        float D = distributionGGX(normal, halfVector, roughness);
        float G   = geometrySmith(normal, viewDirection_viewSpace, lightVector_viewSpace, roughness);
        vec3 F    = fresnelSchlick(max(dot(halfVector, viewDirection_viewSpace), 0.0), F0);
        vec3 Ks = F;
        vec3 Kd = vec3(1.0) - Ks;
        Kd *= 1.0 - metallic;
        
        vec3 specularComp     = D * G * F / 4.0 * max(dot(normal, viewDirection_viewSpace), 0.0) * max(dot(normal, lightVector_viewSpace), 0.0) + 0.0001;
        
        vec3 Lo = vec3(0.0);
        Lo += (Kd * albedo / PI + specularComp) * radiance * max(dot(normal, lightVector_viewSpace), 0.0);
        vec3 Ka;
        
        if(enableDiffuseIrradiance && enableSpecularIrradiance){
            vec3 F    = fresnelSchlickWithRoughness(max(dot(normal, viewDirection_viewSpace), 0.0), F0, roughness);
            vec3 Ks = F;
            vec3 Kd = vec3(1.0) - Ks;
            Kd *= 1.0 - metallic;
            vec3 irradiance = texture(irradianceMap, normal).rgb;
            vec3 diffuse = irradiance * albedo;
            
            vec3 prefilteredColor;
            
            if(int(roughness * maxMipLevels) == 0){
                prefilteredColor = texture(prefilterMap1, reflectionDirection).rgb;
            }else if(int(roughness * maxMipLevels) == 1){
                prefilteredColor = texture(prefilterMap2, reflectionDirection).rgb;
            }else if(int(roughness * maxMipLevels) == 2){
                prefilteredColor = texture(prefilterMap3, reflectionDirection).rgb;
            }else if(int(roughness * maxMipLevels) == 3){
                prefilteredColor = texture(prefilterMap4, reflectionDirection).rgb;
            }else if(int(roughness * maxMipLevels) == 4){
                prefilteredColor = texture(prefilterMap5, reflectionDirection).rgb;
            }
            vec2 brdfValue  = texture(brdfTexture, vec2(max(dot(normal, viewDirection_viewSpace), 0.0), roughness)).rg;
            vec3 specular = prefilteredColor * (F * brdfValue.x + brdfValue.y);
            Ka = (Kd * diffuse + specular) * ao;
        }else if(enableDiffuseIrradiance){
            vec3 F    = fresnelSchlickWithRoughness(max(dot(normal, viewDirection_viewSpace), 0.0), F0, roughness);
            vec3 Ks = F;
            vec3 Kd = vec3(1.0) - Ks;
            Kd *= 1.0 - metallic;
            vec3 irradiance = texture(irradianceMap, normal).rgb;
            vec3 diffuse = irradiance * albedo;
            Ka = Kd * diffuse * ao;
        }else{
            Ka = vec3(0.03) * albedo * ao;
        }
        vColor = Ka + Lo;
        vColor = vColor / (vColor + vec3(1.0));
        //Gamma correction
        vColor = pow(vColor, vec3(1.0/2.2));
    }else{
        vec3 Kd = vec3(1.00f, 0.00f, 0.00f);
        vec3 Ks = vec3(0.8, 0.8, 0.8);
        float alpha = 5.0;
        vec3 Ka = vec3(1.00f, 0.00f, 0.00f);
        vec3 I = vec3(0.4f, 0.4f, 0.4f);
        
        vec3 diffuseComp = max(dot(normal, lightVector_viewSpace), 0.0) * Kd;
        vec3 specularComp = pow(max(dot(normal, halfVector), 0.0), alpha) * Ks;
        vColor = I * (Ka +(diffuseComp + specularComp));
    }
    color = vec4(vColor, 1.0f);
}


float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}


float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

float geometrySchlickGGX(float dotProduct, float roughness)
{
    float k = (roughness * roughness) / 2.0;
    return dotProduct / dotProduct * (1.0 - k) + k;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickWithRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
