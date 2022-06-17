//
//  main.cpp
//  ICG
//
//  Created by Santhosh Natarajappa.
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
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Project 5 - Render Buffers", nullptr, nullptr);
    
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Intializing window is complete.
    // Started creating teapot buffers
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    
    //vertex buffers
    objTriMesh.LoadFromFileObj(fileName,GL_TRUE);
    objTriMesh.ComputeNormals();
    
    for(int i=0; i < objTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            vertexIndices.push_back(objTriMesh.V(objTriMesh.F(i).v[j]));
            normalIndices.push_back(objTriMesh.VN(objTriMesh.FN(i).v[j]));
            textureIndices.push_back(objTriMesh.VT(objTriMesh.FT(i).v[j]));
        }
    }
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*vertexIndices.size(), &vertexIndices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*normalIndices.size(), &normalIndices[0], GL_STATIC_DRAW);
    
    //Texture buffers
    glGenBuffers(1, &texturebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*textureIndices.size(), &textureIndices[0], GL_STATIC_DRAW);
    //Reading texture from image starts
    unsigned imageWidth = 512;
    unsigned imageHeight = 512;
    std::vector<unsigned char> diffuseImageData;
    std::string diffuseFile(objTriMesh.M(0).map_Kd);
    lodepng::decode(diffuseImageData, imageWidth, imageHeight, "resources/objects/" + diffuseFile);
    
    GLuint diffuseTextureID;
    glGenTextures(1, &diffuseTextureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &diffuseImageData[0]);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    std::vector<unsigned char> specularImageData;
    std::string specularFile(objTriMesh.M(0).map_Ks);
    lodepng::decode(specularImageData, imageWidth, imageHeight, "resources/objects/" + specularFile);
    
    GLuint specularTextureID;
    glGenTextures(1, &specularTextureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &specularImageData[0]);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //Reading textures from image completes
    
    objGLSLProgram.BuildFiles("resources/shaders/teapot.vs", "resources/shaders/teapot.fs", nullptr, nullptr, nullptr, &std::cout);
    //Creating teapot buffers is complete
    
    //Started creatiing frame buffer
    //Framebuffer
    renderBuffer.Initialize(GL_TRUE, 4, screenWidth, screenHeight);
    renderBuffer.BuildTextureMipmaps();
    renderBuffer.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    renderBuffer.SetTextureMaxAnisotropy();
    
    GLuint plane_vao;
    glGenVertexArrays(1, &plane_vao);
    glBindVertexArray(plane_vao);
    
    static const GLfloat plane_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    
    GLuint plane_vertexBuffer;
    glGenBuffers(1, &plane_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, plane_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertex_buffer_data), plane_vertex_buffer_data, GL_STATIC_DRAW);
    
    planeGLSLProgram.BuildFiles("resources/shaders/plane.vs", "resources/shaders/plane.fs", nullptr, nullptr, nullptr, &std::cout);
    // Framebuffer creation is complete
    while (!glfwWindowShouldClose(window)) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LESS);
        glEnable(GL_TEXTURE_2D);
        glUseProgram(objGLSLProgram.GetID());
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
        //Texture logic start
        glUniform1i(glGetUniformLocation(objGLSLProgram.GetID(), "diffuseTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 1);
        glUniform1i(glGetUniformLocation(objGLSLProgram.GetID(), "specularTexture"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 2);
        //Texture logic end
        
        //teapot rendering starts
        renderBuffer.Bind();
        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clearing the buffer with the preset color
        
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
        
        // 3rd attribute buffer : textures
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
        glVertexAttribPointer(
                              2,                                // attribute
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*objTriMesh.NV());
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        renderBuffer.Unbind();
        // teapot rendering ends
        // Plane rendering starts
        glUseProgram(planeGLSLProgram.GetID());
        glm::mat4 planeMVPMatrix = getPlaneMVPMatrix();
        glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "planeMVPMatrix"), 1, GL_FALSE, &planeMVPMatrix[0][0]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,screenWidth,screenHeight);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform1i(glGetUniformLocation(planeGLSLProgram.GetID(), "renderedTexture"), 0);
        renderBuffer.BindTexture(0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, plane_vertexBuffer);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
        // Plane rendering ends
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &texturebuffer);
    glDeleteBuffers(1, &plane_vertexBuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &plane_vao);
    glDeleteProgram(objGLSLProgram.GetID());
    glDeleteProgram(planeGLSLProgram.GetID());
    
    glfwTerminate();
    return 0;
}

