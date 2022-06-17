//
//  transformations.h
//  Transformations
//
//  Created by Santhosh Natarajappa.
//

#ifndef transformations_h
#define transformations_h

void computeMatricesFromInputs(GLFWwindow *window){
    if(!initialized){
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, 0.05, 0.05));
        viewMatrix =glm::lookAt(cameraPos, cameraTarget, cameraUp);
        projectionMatrix = glm::perspective( glm::radians(fov),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
        //Light View
        lightViewMatrix = glm::lookAt(lightPosition, lightTarget, lightUp);
        lightProjectionMatrix = glm::perspective( glm::radians(lightFOV),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
        initialized = true;
    }else if (currMouseX != prevMouseX || currMouseY != prevMouseY) {
        if(rotateCamera){
            glm::mat4 vRotationMatrix = glm::rotate(glm::radians(verticalAngle), glm::vec3(1, 0, 0));
            glm::mat4 hRotationMatrix = glm::rotate(glm::radians(-horizontalAngle), glm::vec3(0, 1, 0));
            cameraPos =hRotationMatrix * glm::vec4(cameraPos,1);
            cameraPos =vRotationMatrix * glm::vec4(cameraPos,1);
            viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
            prevMouseX = currMouseX;
            prevMouseY = currMouseY;
        }else if (rotateLight)
        {
            glm::mat4 vRotationMatrix = glm::rotate(glm::radians(verticalAngle), glm::vec3(1, 0, 0));
            glm::mat4 hRotationMatrix = glm::rotate(glm::radians(-horizontalAngle), glm::vec3(0, 1, 0));
            lightPosition = hRotationMatrix * glm::vec4(lightPosition,1);
            lightPosition = vRotationMatrix * glm::vec4(lightPosition,1);
            lightViewMatrix = glm::lookAt(lightPosition, lightTarget, lightUp);
            prevMouseX = currMouseX;
            prevMouseY = currMouseY;
        }else if(zoomCamera){
            projectionMatrix = glm::perspective( glm::radians(fov),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
            prevMouseX = currMouseX;
            prevMouseY = currMouseY;
        }
    }
}

void onKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GL_TRUE);
    }else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS){
        controlKeyPressed = true;
    }else if (key == GLFW_KEY_P && action == GLFW_PRESS){
        enablePBR = !enablePBR;
    }else if (key == GLFW_KEY_T && action == GLFW_PRESS){
        enableTexture = !enableTexture;
    }else if (key == GLFW_KEY_D && action == GLFW_PRESS){
        enableDiffuseIrradiance = !enableDiffuseIrradiance;
    }else if (key == GLFW_KEY_S && action == GLFW_PRESS){
        enableSpecularIrradiance = !enableSpecularIrradiance;
    }else if (key == GLFW_KEY_UP && action == GLFW_PRESS && metallic < 1.0f){
        metallic += 0.1f;
    }else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && metallic > 0.0f){
        metallic -= 0.1f;
    }else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS && roughness < 1.0f){
        roughness += 0.1f;
    }else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS && roughness > 0.0f){
        roughness -= 0.1f;
    }else if (key == GLFW_KEY_1 && action == GLFW_PRESS){
        materialFilePath = "resources/materials/porcelain-white/";
        computeMaterials();
    }else if (key == GLFW_KEY_2 && action == GLFW_PRESS){
        materialFilePath = "resources/materials/bamboo-wood/";
        computeMaterials();
    }else if (key == GLFW_KEY_3 && action == GLFW_PRESS){
        materialFilePath = "resources/materials/paint-peeling/";
        computeMaterials();
    }else if (key == GLFW_KEY_4 && action == GLFW_PRESS){
        materialFilePath = "resources/materials/gold-scuffed/";
        computeMaterials();
    }else if (key == GLFW_KEY_5 && action == GLFW_PRESS){
        materialFilePath = "resources/materials/dirty-gold/";
        computeMaterials();
    }else if (key == GLFW_KEY_6 && action == GLFW_PRESS){
        materialFilePath = "resources/materials/handled-pot/";
        computeMaterials();
    }else if (key == GLFW_KEY_7 && action == GLFW_PRESS){
        materialFilePath = "resources/materials/dull-brass/";
        computeMaterials();
    }else if (key == GLFW_KEY_8 && action == GLFW_PRESS){
        cubeMapFileName = "resources/envmap/hdr/goldroom/Mt-Washington-Gold-Room_Ref.hdr";
        computeEnvMap();
    }else if (key == GLFW_KEY_9 && action == GLFW_PRESS){
        cubeMapFileName = "resources/envmap/hdr/cloudysky/HDR_029_Sky_Cloudy_Ref.hdr";
        computeEnvMap();
    }else if (key == GLFW_KEY_0 && action == GLFW_PRESS){
        cubeMapFileName = "resources/envmap/hdr/nightcity/HDR_Free_City_Night_Lights_Ref.hdr";
        computeEnvMap();
    }else if (key == GLFW_KEY_F6 && action == GLFW_PRESS){
        objGLSLProgram.BuildFiles("resources/shaders/object.vs", "resources/shaders/object.fs", nullptr, nullptr, nullptr, &std::cout);
        equirectangularToCubemapGLSLProgram.BuildFiles("resources/shaders/cubemap.vs", "resources/shaders/equirectagular_to_cubemap.fs", nullptr, nullptr, nullptr, &std::cout);
        irradianceGLSLProgram.BuildFiles("resources/shaders/cubemap.vs", "resources/shaders/irradiance_to_cubemap.fs", nullptr, nullptr, nullptr, &std::cout);
        prefilterGLSLProgram.BuildFiles("resources/shaders/cubemap.vs", "resources/shaders/prefilter_to_cubemap.fs", nullptr, nullptr, nullptr, &std::cout);
        brdfGLSLProgram.BuildFiles("resources/shaders/brdf.vs", "resources/shaders/brdf.fs", nullptr, nullptr, nullptr, &std::cout);
        envMapGLSLProgram.BuildFiles("resources/shaders/envmap.vs", "resources/shaders/envmap.fs", nullptr, nullptr, nullptr, &std::cout);
    }
}

void onMouseButtonPress(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if(controlKeyPressed){
            rotateLight = true;
        }else{
            rotateCamera = true;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
            zoomCamera = true;
    }
    if (action == GLFW_RELEASE)
    {
        rotateCamera = false;
        zoomCamera = false;
        controlKeyPressed = false;
        rotateLight = false;
        firstMouse = true;
    }
}

void onMouseMovement(GLFWwindow* window, double xPos, double yPos)
{
    if (rotateCamera || zoomCamera || rotateLight)
    {
        if (firstMouse)
        {
            prevMouseX = xPos;
            prevMouseY = yPos;
            firstMouse = false;
        }
        currMouseX = xPos;
        currMouseY = yPos;
        float xoffset = currMouseX - prevMouseX;
        float yoffset = currMouseY - prevMouseY;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        
        horizontalAngle   = xoffset;
        verticalAngle = yoffset;
        
        if(zoomCamera){
            fov += (float)yoffset;
            if (fov < 1.0f)
                fov = 1.0f;
            if (fov > 100.0f)
                fov = 100.0f;
        }
    }
}

void checkGLError()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR){
        std::cout << err << std::endl;
    }
}

#endif /* transformations_h */
