//
//  main.cpp
//  ICG
//
//  Created by Santhosh Natarajappa.
//

#include "transformations.h"
#include "render.h"


int main(int argc, char** argv) {
    // .obj file as its first command-line argument
    if (argc == 2){
        objectFileName.append(argv[1]);
    }else{
        objectFileName.append("teapot.obj");
        std::cout << "No argument is passed. Using default object file => " << objectFileName << std::endl;
    }
    const char *fileName = objectFileName.c_str();
    // Initialise GLFW
    if( !glfwInit() )
    {
        std::cout << "Failed to initialize glfw" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Project 7 - Shadow Mapping", nullptr, nullptr);
    
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
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Intializing window is complete.
    
    //Compile all the shaders
    objGLSLProgram.BuildFiles("resources/shaders/teapot.vs", "resources/shaders/teapot.fs", nullptr, nullptr, nullptr, &std::cout);
    planeGLSLProgram.BuildFiles("resources/shaders/plane.vs", "resources/shaders/plane.fs", nullptr, nullptr, nullptr, &std::cout);
    shadowGLSLProgram.BuildFiles("resources/shaders/shadow.vs", "resources/shaders/shadow.fs", nullptr, nullptr, nullptr, &std::cout);
    lightGLSLProgram.BuildFiles("resources/shaders/light.vs", "resources/shaders/light.fs", nullptr, nullptr, nullptr, &std::cout);
    
    
    // Started creating teapot buffers
    initializeTeapotBuffer(fileName);
    
    initializeTeapotShadowBuffer(fileName);
    //Creating teapot buffers is complete
    //Started creatiing frame buffer
    renderDepth.Initialize(GL_FALSE, screenWidth, screenHeight);
    renderDepth.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
    renderDepth.SetTextureWrappingMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    // Framebuffer creation is complete
    std::string planeObjectFile = "resources/objects/smallplane.obj";
    initializePlaneBuffer(planeObjectFile.c_str());
    
    std::string lightFileName = "resources/objects/sphere.obj";
    initializeLightBuffer(lightFileName.c_str());
    
    while (!glfwWindowShouldClose(window)) {
        computeMatricesFromInputs(window);
        //rendering teapot reflection starts
        glViewport(0, 0, screenWidth, screenHeight);
        shadowGLSLProgram.Bind();
        sendUniformsToTeapotShadowShaders();
        renderDepth.Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        renderTeapotShadow();
        glCullFace(GL_BACK);
        renderDepth.Unbind();
        //rendering depth map ends
        glDisable(GL_CULL_FACE);
        glViewport(0, 0, screenWidth, screenHeight);
        objGLSLProgram.Bind();
        sendUniformsToTeapotShaders();
        //teapot rendering starts
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color
        renderDepth.BindTexture(0);
        renderTeapot();
        // teapot rendering ends
        planeGLSLProgram.Bind();
        sendUniformsToPlaneShaders();
        renderDepth.BindTexture(0);
        renderPlane();
        // Plane rendering ends
        
        lightGLSLProgram.Bind();
        sendUniformsToLightShaders();
        renderLight();
        // light rendering ends
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        checkGLError();
    }
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &planeVertexbuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &plane_vao);
    glDeleteProgram(objGLSLProgram.GetID());
    glDeleteProgram(planeGLSLProgram.GetID());
    
    glfwTerminate();
    return 0;
}

