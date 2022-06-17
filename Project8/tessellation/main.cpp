//
//  main.cpp
//  ICG
//
//  Created by Santhosh Natarajappa.
//

#include "transformations.h"


void initializeObjectBuffer(const char *fileName){
    //vertex buffers
    objTriMesh.LoadFromFileObj(fileName,GL_TRUE);
    objTriMesh.ComputeNormals();
    
    for(int i=0; i < objTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            vertexIndices.push_back(objTriMesh.V(objTriMesh.F(i).v[j]));
            normalIndices.push_back(objTriMesh.VN(objTriMesh.FN(i).v[j]));
        }
    }
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*vertexIndices.size(), &vertexIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*normalIndices.size(), &normalIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
                          1,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
}

void sendUniformsToShaders(cyGLSLProgram program){
    glm::mat4 model = modelMatrix;
    glm::mat4 view = viewMatrix;
    glm::mat4 projection = projectionMatrix;
    glm::mat4 mvpMatrix = projection * view * model;
    glm::mat4 mvMatrix = getMVMatrix();
    glm::mat4 normalMVMatrix = getNormalMVMatrix();
    glUniformMatrix4fv(glGetUniformLocation(program.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program.GetID(), "normalMVMatrix"), 1, GL_FALSE, &normalMVMatrix[0][0]);
    glUniform3f(glGetUniformLocation(program.GetID(), "cameraPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
    
    glUniform3f(glGetUniformLocation(program.GetID(), "lightPosition_viewSpace"), lightPosition.x, lightPosition.y, lightPosition.z);
    
    program.SetUniform("tessellationLevel", tessellationLevel);
    program.SetUniform("displacementLevel", displacementLevel);
    
    glm::mat4 lpMatrix = lightProjectionMatrix * lightViewMatrix;
    glUniformMatrix4fv(glGetUniformLocation(program.GetID(), "lpMatrix"), 1, GL_FALSE, &lpMatrix[0][0]);
}

void sendUniformsToObjectShaders(){
    glm::mat4 model = modelMatrix;
    glm::mat4 view = viewMatrix;
    glm::mat4 projection = projectionMatrix;
    glm::mat4 mvpMatrix = projection * view * model;
    glm::mat4 mvMatrix = getMVMatrix();
    glm::mat4 normalMVMatrix = getNormalMVMatrix();
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "normalMVMatrix"), 1, GL_FALSE, &normalMVMatrix[0][0]);
    glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "cameraPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
    
    //Light arrangement
    glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition.x, lightPosition.y, lightPosition.z);
    std::cout << "light position:: " << glm::to_string(lightPosition)<< std::endl;

    objGLSLProgram.SetUniform("tessellationLevel", tessellationLevel);
    objGLSLProgram.SetUniform("displacementLevel", displacementLevel);
    
    glm::mat4 lpMatrix = lightProjectionMatrix * lightViewMatrix;
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "lpMatrix"), 1, GL_FALSE, &lpMatrix[0][0]);
}

void renderObject(){
    glBindVertexArray(vao);
    //    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*objTriMesh.NV());
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, sizeof(cyVec3f)*objTriMesh.NV());
    glBindVertexArray(0);
}

void renderStrip(){
    glBindVertexArray(vao);
    //    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*objTriMesh.NV());
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, sizeof(cyVec3f)*objTriMesh.NV());
    glBindVertexArray(0);
}

void sendUniformsToStripShaders(){    
    glm::mat4 model = modelMatrix;
    glm::mat4 view = viewMatrix;
    glm::mat4 projection = projectionMatrix;
    glm::mat4 mvpMatrix = projection*view*model;
    glm::mat4 mvMatrix = getMVMatrix();
    glm::mat4 normalMVMatrix = getNormalMVMatrix();
    glUniformMatrix4fv(glGetUniformLocation(stripGLSLProgram.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(stripGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(stripGLSLProgram.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(stripGLSLProgram.GetID(), "normalMVMatrix"), 1, GL_FALSE, &normalMVMatrix[0][0]);
    glUniform3f(glGetUniformLocation(stripGLSLProgram.GetID(), "cameraPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
    
    //Light arrangement
    glUniform3f(glGetUniformLocation(stripGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition.x, lightPosition.y, lightPosition.z);
    
    stripGLSLProgram.SetUniform("tessellationLevel", tessellationLevel);
    stripGLSLProgram.SetUniform("displacementLevel", displacementLevel);
    
    glm::mat4 lpMatrix = lightProjectionMatrix * lightViewMatrix;
    glUniformMatrix4fv(glGetUniformLocation(stripGLSLProgram.GetID(), "lpMatrix"), 1, GL_FALSE, &lpMatrix[0][0]);
}

GLuint getTexture(std::string fileName){
    unsigned imageWidth = 800;
    unsigned imageHeight = 800;
    std::vector<unsigned char> imageData;
    std::string file(fileName);
    lodepng::decode(imageData, imageWidth, imageHeight, file);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageData[0]);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return textureID;
}


void initializeTeapotShadowBuffer(const char *fileName){
    shadowTriMesh.LoadFromFileObj(fileName,GL_TRUE);
    shadowTriMesh.ComputeNormals();
    
    for(int i=0; i < shadowTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            shadowVertexIndices.push_back(shadowTriMesh.V(shadowTriMesh.F(i).v[j]));
            shadowNormalIndices.push_back(shadowTriMesh.VN(shadowTriMesh.FN(i).v[j]));
        }
    }
    
    glGenVertexArrays(1, &shadow_vao);
    glBindVertexArray(shadow_vao);
    glGenBuffers(1, &shadowVertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, shadowVertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*shadowVertexIndices.size(), &shadowVertexIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    glGenBuffers(1, &shadowNormalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, shadowNormalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*shadowNormalIndices.size(), &shadowNormalIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
                          1,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
}

void sendUniformsToTeapotShadowShaders(){
    glm::mat4 model = modelMatrix;
    glm::mat4 lightView = lightViewMatrix;
    glm::mat4 lightProjection = lightProjectionMatrix;
    glm::mat4 lpMatrix = lightProjection * lightView;
    glUniformMatrix4fv(glGetUniformLocation(shadowGLSLProgram.GetID(), "lpMatrix"), 1, GL_FALSE, &lpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shadowGLSLProgram.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    
    shadowGLSLProgram.SetUniform("tessellationLevel", tessellationLevel);
    shadowGLSLProgram.SetUniform("displacementLevel", displacementLevel);
}


void renderTeapotShadow(){
    glBindVertexArray(shadow_vao);
    //    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*shadowTriMesh.NV());
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, sizeof(cyVec3f)*objTriMesh.NV());
    glBindVertexArray(0);
}

int main(int argc, char** argv) {
    // .obj file as its first command-line argument
    if (argc == 2){
        normalMappingFileName.append(argv[1]);
        tessellationLevel = 0.125;
    }else if (argc == 3){
        normalMappingFileName.append(argv[1]);
        dispMappingFileName.append(argv[2]);
    }else{
        normalMappingFileName.append("teapot_normal.png");
        dispMappingFileName.append("teapot_disp.png");
    }
    objectFileName.append("plane.obj");
    const char *fileName = objectFileName.c_str();
    const char *normalFileName = normalMappingFileName.c_str();
    const char *dispFileName = dispMappingFileName.c_str();
    // Initialise GLFW
    if( !glfwInit() )
    {
        std::cout << "Failed to initialize glfw" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Project 8 - Tessellation", nullptr, nullptr);
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
    glEnable(GL_CULL_FACE);
    
    GLuint normalMapTexID =  getTexture(normalFileName);
    GLuint dispMapTexID =  getTexture(dispFileName);
    
    objGLSLProgram.BuildFiles("resources/shaders/strip_vs.vs", "resources/shaders/object_fs.fs", nullptr, "resources/shaders/strip_tcs.vs", "resources/shaders/strip_tes.vs", &std::cout);
    stripGLSLProgram.BuildFiles("resources/shaders/strip_vs.vs", "resources/shaders/strip_fs.fs", "resources/shaders/strip_gs.gs", "resources/shaders/strip_tcs.vs", "resources/shaders/strip_tes.vs", &std::cout);
    
    shadowGLSLProgram.BuildFiles("resources/shaders/shadow.vs", "resources/shaders/shadow.fs", nullptr, "resources/shaders/shadow_tcs.vs", "resources/shaders/shadow_tes.vs", &std::cout);
    
    objGLSLProgram.Bind();
    objGLSLProgram.SetUniform("normalMap", 0);
    objGLSLProgram.SetUniform("dispMap", 1);
    objGLSLProgram.SetUniform("shadowMap", 2);
    
    
    stripGLSLProgram.Bind();
    stripGLSLProgram.SetUniform("normalMap", 0);
    stripGLSLProgram.SetUniform("dispMap", 1);
    stripGLSLProgram.SetUniform("shadowMap", 2);
    
    shadowGLSLProgram.Bind();
    objGLSLProgram.SetUniform("normalMap", 0);
    shadowGLSLProgram.SetUniform("dispMap", 1);
    
    initializeObjectBuffer(fileName);
    
    initializeTeapotShadowBuffer(fileName);
    //Creating teapot buffers is complete
    //Started creatiing frame buffer
    renderDepth.Initialize(GL_FALSE, screenWidth, screenHeight);
    renderDepth.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
    renderDepth.SetTextureWrappingMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    // Framebuffer creation is complete
    
    
    while (!glfwWindowShouldClose(window)) {
        computeMatricesFromInputs(window);
        //rendering teapot reflection starts
        if(renderShadow){
            glViewport(0, 0, screenWidth, screenHeight);
            shadowGLSLProgram.Bind();
            sendUniformsToTeapotShadowShaders();
            renderDepth.Bind();
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            renderTeapotShadow();
            glCullFace(GL_BACK);
            renderDepth.Unbind();
        }
        //rendering depth map ends
        glDisable(GL_CULL_FACE);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,screenWidth,screenHeight);
        
        objGLSLProgram.Bind();
        sendUniformsToObjectShaders();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normalMapTexID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dispMapTexID);
        renderDepth.BindTexture(2);
        renderObject();
        
        if(renderStrips){
            stripGLSLProgram.Bind();
            //            sendUniformsToShaders(stripGLSLProgram);
            sendUniformsToStripShaders();
            renderStrip();
        }
        
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


