//Plane fragment shader
#version 330 core

out vec4 color;

in vec3 positionVector_worldSpace;
in vec3 normalVector_worldSpace;

in vec4 positionVector_viewSpace;
in vec3 normalVector_viewSpace;
in vec3 lightVector_viewSpace;

uniform vec3 cameraPosition;

uniform sampler2D renderedTexture;
uniform samplerCube skybox;

void main(){
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
    vec3 incidentVector = normalize(positionVector_worldSpace - cameraPosition);
    vec3 relectionVector = reflect(incidentVector, normalize(normalVector_worldSpace));
    vec4 envColor = texture(skybox, relectionVector);
    vec4 textureColor =texture(renderedTexture, gl_FragCoord.xy / textureSize(renderedTexture, 0));
    vec3 vColor;
    if(length(textureColor.rbg) == 0){
        vColor = I * (Ka + diffuseComp)* envColor.rgb + specularComp;
    }else{
        vColor = I * (Ka + diffuseComp)* textureColor.rgb + specularComp;
    }
    color = vec4(vColor, 1.0f);
}
