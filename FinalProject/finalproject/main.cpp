//
//  main.cpp
//  ICG
//
//  Created by Santhosh Natarajappa.
//


#include "render.h"
#include "transformations.h"

int main(int argc, char** argv) {
    // .obj file as its first command-line argument
    if (argc == 2){
        objectFileName.append(argv[1]);
    }else{
        objectFileName.append("teapot.obj");
        std::cout << "No argument is passed. Using default object file => " << objectFileName << std::endl;
    }
    const char *fileName = objectFileName.c_str();
    if( !glfwInit() )
    {
        std::cout << "Failed to initialize glfw" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Final Project - Image Based Lighting", nullptr, nullptr);
    
    glfwSetKeyCallback(window, onKeyAction);
    glfwSetCursorPosCallback(window, onMouseMovement);
    glfwSetMouseButtonCallback(window, onMouseButtonPress);
    
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    if(nullptr == window){
        glfwTerminate();
        std::cout << "Failed to create window" << std::endl;
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    
    if(GLEW_OK != glewInit()){
        std::cout << "Failed to initialize  GLEW" << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, screenWidth, screenHeight);

    initializeCubeBuffer();
    initializeQuadBuffers();
    computeMaterials();
    objGLSLProgram.BuildFiles("resources/shaders/object.vs", "resources/shaders/object.fs", nullptr, nullptr, nullptr, &std::cout);
    objGLSLProgram.Bind();
    objGLSLProgram.SetUniform("albedoMap", 0);
    objGLSLProgram.SetUniform("metallicMap", 1);
    objGLSLProgram.SetUniform("roughnessMap", 2);
    objGLSLProgram.SetUniform("aoMap", 3);
    objGLSLProgram.SetUniform("normalMap", 4);
    objGLSLProgram.SetUniform("irradianceMap", 5);
    objGLSLProgram.SetUniform("prefilterMap1", 6);
    objGLSLProgram.SetUniform("prefilterMap2", 7);
    objGLSLProgram.SetUniform("prefilterMap3", 8);
    objGLSLProgram.SetUniform("prefilterMap4", 9);
    objGLSLProgram.SetUniform("prefilterMap5", 10);
    objGLSLProgram.SetUniform("brdfTexture", 11);
    
    equirectangularToCubemapGLSLProgram.BuildFiles("resources/shaders/cubemap.vs", "resources/shaders/equirectagular_to_cubemap.fs", nullptr, nullptr, nullptr, &std::cout);
    irradianceGLSLProgram.BuildFiles("resources/shaders/cubemap.vs", "resources/shaders/irradiance_to_cubemap.fs", nullptr, nullptr, nullptr, &std::cout);
    prefilterGLSLProgram.BuildFiles("resources/shaders/cubemap.vs", "resources/shaders/prefilter_to_cubemap.fs", nullptr, nullptr, nullptr, &std::cout);
    brdfGLSLProgram.BuildFiles("resources/shaders/brdf.vs", "resources/shaders/brdf.fs", nullptr, nullptr, nullptr, &std::cout);
    
    envMapGLSLProgram.BuildFiles("resources/shaders/envmap.vs", "resources/shaders/envmap.fs", nullptr, nullptr, nullptr, &std::cout);
    envMapGLSLProgram.Bind();
    envMapGLSLProgram.SetUniform("environmentMap", 0);

    computeEnvMap();
    brdfTexture = createBRDFTextureForSampling();

    initializeObjectBuffer(fileName);
    
    while (!glfwWindowShouldClose(window)) {
        computeMatricesFromInputs(window);
        //teapot rendering starts
        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color
        objGLSLProgram.Bind();
        sendUniformsToObjectShaders();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoMapTexID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metallicMapTexID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, roughnessMapTexID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, aoMapTexID);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, normalMapTexID);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceCubemap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemap1);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemap2);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemap3);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemap4);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemap5);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, brdfTexture);
        renderObject();
        // teapot rendering ends
        
        //render env map - starts
        envMapGLSLProgram.Bind();
        glUniformMatrix4fv(glGetUniformLocation(envMapGLSLProgram.GetID(), "view"), 1, GL_FALSE, &viewMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(envMapGLSLProgram.GetID(), "projection"), 1, GL_FALSE, &projectionMatrix[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        renderCube();
        //render env map - ends
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        checkGLError();
    }
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &texturebuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(objGLSLProgram.GetID());
    glDeleteProgram(envMapGLSLProgram.GetID());
    glDeleteProgram(equirectangularToCubemapGLSLProgram.GetID());
    
    glfwTerminate();
    return 0;
}

