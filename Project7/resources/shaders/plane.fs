//Fragment shader
#version 330 core
layout(location = 0) out vec4 color;

in vec4 positionVector_viewSpace;
in vec3 normalVector_viewSpace;
in vec3 lightVector_viewSpace;


in vec4 fragPosition_lightSpace;
in vec3 normal_forBias;
in vec3 lightPos_forBias;
in vec3 FragPos_forBias;

uniform sampler2D shadowMap;

float computeShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5 + 0.5;
    float textureDepth = texture(shadowMap, projCoords.xy).r;
    float depth = projCoords.z;
    float bias = 0.000001;
    bias = max (0.00001* (1.0 - dot(normalize(normal_forBias), normalize(lightPos_forBias - FragPos_forBias))), bias);
    float shadow = depth - bias > textureDepth  ? 1.0 : 0.0;
    
    if(depth > 1.0){
        shadow = 0.0;
    }
    return shadow;
}



void main()
{
    vec3 Kd = vec3(0.416f, 0.545f, 0.588f);
    vec3 Ks = vec3(0.8, 0.8, 0.8);
    float alpha = 50.0;
    vec3 Ka = vec3(0.416f, 0.545f, 0.588f);
    vec3 I = vec3(0.4, 0.4, 0.4);

    //View direction is opposit of direction from the camera normalized
    vec3 viewDirection_viewSpace = normalize(-positionVector_viewSpace.xyz);
    //Add view direction + light direction and normalize
    vec3 halfVector = normalize(lightVector_viewSpace + viewDirection_viewSpace);
    vec3 diffuseComp = max(dot(normalVector_viewSpace, lightVector_viewSpace), 0.0) * Kd;
    vec3 specularComp = pow(max(dot(normalVector_viewSpace, halfVector), 0.0), alpha) * Ks;

    // calculate shadow
    float shadow = computeShadow(fragPosition_lightSpace);
//    shadow = 0;
    vec3 vColor = I * (Ka + (1- shadow) * (diffuseComp + specularComp));
    color = vec4(vColor, 1.0f);
}


