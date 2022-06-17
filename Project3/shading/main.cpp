//
//  main.cpp
//  ICG
//
//  Created by Santhosh Natarajappa on 12/22/21.
//

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
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Project 3 - Shading", nullptr, nullptr);
    
    glfwSetKeyCallback(window, onKeyAction);
    glfwSetCursorPosCallback(window, onMouseMovement);
    glfwSetMouseButtonCallback(window, onMouseButtonPress);
    
    int screenWidth, screenHeight;
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
    
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    
    //vertex buffers
    objTriMesh.LoadFromFileObj(fileName,GL_FALSE);
    objTriMesh.ComputeNormals();
    
    for(int i=0; i < objTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            vertexIndices.push_back(objTriMesh.V(objTriMesh.F(i).v[j]));
            normalIndices.push_back(objTriMesh.VN(objTriMesh.FN(i).v[j]));
        }
    }
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*vertexIndices.size(), &vertexIndices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*normalIndices.size(), &normalIndices[0], GL_STATIC_DRAW);
    
    //normal buffers
    
    objGLSLProgram.BuildFiles("resources/shaders/core.vs", "resources/shaders/core.fs", nullptr, nullptr, nullptr, &std::cout);
    
    while (!glfwWindowShouldClose(window)) {
        //Transformation Logic
        computeMatricesFromInputs(window);
        glm::mat4 mvpMatrix = getMVPMatrix();
        glm::mat4 mvMatrix = getMVMatrix();
        glm::mat4 normalMVMatrix = getNormalMVMatrix();
        glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "normalMVMatrix"), 1, GL_FALSE, &normalMVMatrix[0][0]);
        
        //Light arrangement
        glm::mat4 lightMVMatrix = getLightMVMatrix();
        glm::vec3 lightPosition = glm::vec3(100.0f, 50.0f, 0.0f);
        glm::vec3 lightPosition_viewSpace = (lightMVMatrix * vec4(lightPosition, 1.0));
        glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition_viewSpace.x, lightPosition_viewSpace.y, lightPosition_viewSpace.z);
        
        //Transformation logic end
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color
        // Use our shader
        glUseProgram(objGLSLProgram.GetID());
        
        // 1st attribute buffer : vertex
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // 2nd attribute buffer : normals
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
                              1,                                // attribute
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*objTriMesh.NV());
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(objGLSLProgram.GetID());
    
    return 0;
}
