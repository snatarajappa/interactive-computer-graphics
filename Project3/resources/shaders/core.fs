//Fragment shader
#version 330 core
in vec4 positionVector_viewSpace;
in vec3 normalVector_viewSpace;
in vec3 lightVector_viewSpace;

out vec4 color;

vec3 Kd = vec3(0.50980395, 0.0, 0.0);
vec3 Ks = vec3(0.80099994, 0.80099994, 0.80099994);
float alpha = 5.0;
vec3 Ka = vec3(0.50980395, 0.0, 0.0);
vec3 I = vec3(1.0, 1.0, 1.0);

void main()
{
    //View direction is opposit of direction from the camera normalized
    vec3 viewDirection_viewSpace = normalize(-positionVector_viewSpace.xyz);
    //Add view direction + light direction and normalize
    vec3 halfVector = normalize(lightVector_viewSpace + viewDirection_viewSpace);
    vec3 diffuseComp = max(dot(normalVector_viewSpace, lightVector_viewSpace), 0.0) * Kd;
    vec3 specularComp = pow(max(dot(normalVector_viewSpace, halfVector), 0.0), alpha) * Ks;
    
    //    vec3 vColor = normalVector_viewSpace;
    //    vec3 vColor = I * (Ka + (diffuseComp));
    //    vec3 vColor = I * (specularComp);
    vec3 vColor = I * (Ka + (diffuseComp + specularComp));
    color = vec4(vColor, 1.0f);
}
