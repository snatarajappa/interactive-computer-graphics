//Fragment shader
#version 330 core
layout(location = 0) out vec4 color;

in vec4 positionVector_viewSpace;
in vec3 normalVector_viewSpace;
in vec3 lightVector_viewSpace;

//in vec3 positionVector_worldSpace;
in vec3 normalVector_worldSpace;

uniform samplerCube skybox;
//uniform vec3 cameraPosition_worldSpace;
in vec3 cameraPosition_worldSpace;

void main()
{
    vec3 Kd = vec3(0.7, 0.7, 0.7);
    vec3 Ks = vec3(0.80099994, 0.80099994, 0.80099994);
    float alpha = 5.0;
    vec3 Ka = vec3(0.7, 0.7, 0.7);
    vec3 I = vec3(0.5, 0.5, 0.5);
    
    //View direction is opposit of direction from the camera normalized
    vec3 viewDirection_viewSpace = normalize(-positionVector_viewSpace.xyz);
    //Add view direction + light direction and normalize
    vec3 halfVector = normalize(lightVector_viewSpace + viewDirection_viewSpace);
    vec3 diffuseComp = max(dot(normalVector_viewSpace, lightVector_viewSpace), 0.0) * Kd;
    vec3 specularComp = pow(max(dot(normalVector_viewSpace, halfVector), 0.0), alpha) * Ks;
    
    //Environment mapping
    vec3 reflectionVector = reflect(-normalize(cameraPosition_worldSpace),normalize(normalVector_worldSpace));
    vec3 vColor = I * (Ka + diffuseComp)* texture(skybox, reflectionVector).rgb + specularComp;
    color = vec4(vColor, 1.0f);
}

