//
//  main.cpp
//  ICG
//
//  Created by Santhosh Natarajappa.
//

#include "transformations.h"
#include "envmapping.h"
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
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Project 6 - Environment Mapping", nullptr, nullptr);
    if(nullptr == window){
        glfwTerminate();
        std::cout << "Failed to create window" << std::endl;
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    glfwSetKeyCallback(window, onKeyAction);
    glfwSetCursorPosCallback(window, onMouseMovement);
    glfwSetMouseButtonCallback(window, onMouseButtonPress);
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    glewExperimental = GL_TRUE;
    
    if(GLEW_OK != glewInit()){
        std::cout << "Failed to initialize  GLEW" << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, screenWidth, screenHeight);
    // Intializing window is complete.
    
    //Compile all the shaders
    objGLSLProgram.BuildFiles("resources/shaders/teapot.vs", "resources/shaders/teapot.fs", nullptr, nullptr, nullptr, &std::cout);
    planeGLSLProgram.BuildFiles("resources/shaders/plane.vs", "resources/shaders/plane.fs", nullptr, nullptr, nullptr, &std::cout);
    cubeGLSLProgram.BuildFiles("resources/shaders/cube.vs", "resources/shaders/cube.fs", nullptr, nullptr, nullptr, &std::cout);
    reflectionGLSLProgram.BuildFiles("resources/shaders/reflection.vs", "resources/shaders/reflection.fs", nullptr, nullptr, nullptr, &std::cout);
    
    // Load Environment and bind it
    std::string cubeObjectFileName = "resources/objects/cube.obj";
    initializeCubeBuffer(cubeObjectFileName.c_str());
    loadCubeMapTextures();
    
    objGLSLProgram.Bind();
    objGLSLProgram.SetUniform("skybox", 0);
    planeGLSLProgram.Bind();
    planeGLSLProgram.SetUniform("skybox", 0);
    cubeGLSLProgram.Bind();
    cubeGLSLProgram.SetUniform("skybox", 0);
    reflectionGLSLProgram.Bind();
    reflectionGLSLProgram.SetUniform("skybox", 0);
    
    
    // Started creating teapot buffers
    initializeTeapotBuffer(fileName);
    //Creating teapot buffers is complete
    //Setup reflection buffer
    initializeReflectionBuffer(fileName);
    //Setup reflection buffer
    //Started creatiing frame buffer
    renderBuffer.Initialize(GL_TRUE, 4, screenWidth, screenWidth);
    // Framebuffer creation is complete
    
    initializePlaneBuffer();
    
    while (!glfwWindowShouldClose(window)) {
        computeMatricesFromInputs(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //Rendering teapot starts
        objGLSLProgram.Bind();
        sendUniformsToTeapotShaders();
        renderTeapot();
        // teapot rendering ends
        //rendering teapot reflection starts
        reflectionGLSLProgram.Bind();
        sendUniformsToReflectionShaders();
        renderBuffer.Bind();
        glViewport(0, 0, screenWidth, screenWidth);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderReflection();
        renderBuffer.Unbind();
        //rendering teapot reflection ends
        // Plane rendering starts
        planeGLSLProgram.Bind();
        sendUniformsToPlaneShaders();
        renderBuffer.BindTexture(1);
        renderPlane();
        // Plane rendering ends
        //cube mapping starts
        glDepthMask(GL_FALSE);
        cubeGLSLProgram.Bind();
        sendUniformsToCubeShaders();
        renderCubeMap();
        glDepthMask(GL_TRUE);
        //cube mapping end
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        checkGLError();
    }
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &plane_vertexBuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &plane_vao);
    glDeleteProgram(objGLSLProgram.GetID());
    glDeleteProgram(planeGLSLProgram.GetID());
    
    glfwTerminate();
    return 0;
}


