//
//  render.h
//  envmapping
//
//  Created by Santhosh Natarajappa on 3/13/22.
//

#ifndef render_h
#define render_h

#define GL_SILENCE_DEPRECATION

#include <iostream>

//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>


//cyCodeBase
#include <cyCodeBase/cyCore.h>
#include <cyCodeBase/cyVector.h>
#include <cyCodeBase/cyMatrix.h>
#include <cyCodeBase/cyTriMesh.h>
#include <cyCodeBase/cyGL.h>

// Include LodePNG
#include <LodePNG/lodepng.h>

// Include STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;

//Initialising the window width and height
const GLint WIDTH=1800, HEIGHT=900;
GLint screenWidth, screenHeight;

GLfloat prevMouseX = 0.0;
GLfloat prevMouseY = 0.0;
GLfloat currMouseX = 0.0;
GLfloat currMouseY = 0.0;

GLfloat sensitivity = 0.2f;

GLboolean firstMouse = true;
//Initialization flags
GLboolean initialized =false;
//Transformation flags
GLboolean rotateCamera =false;
GLboolean zoomCamera = false;
//Light related flags
GLboolean rotateLight =false;
GLboolean controlKeyPressed = false;

GLfloat horizontalAngle=0.0f;
GLfloat verticalAngle=0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat fov = 45.0f;
//Matrix initialization
glm::mat4 modelMatrix = glm::mat4(1.0f);
glm::mat4 viewMatrix = glm::mat4(1.0f);
glm::mat4 projectionMatrix = glm::mat4(1.0f);

//Light Matrices
glm::vec3 lightPosition = glm::vec3(-10.0f, 10.0f, 10.0f);
glm::vec3 lightTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lightUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat lightFOV = 45.0f;
//Matrix initialization
glm::mat4 lightModelMatrix = glm::mat4(1.0f);
glm::mat4 lightViewMatrix = glm::mat4(1.0f);
glm::mat4 lightProjectionMatrix = glm::mat4(1.0f);

cy::GLSLProgram objGLSLProgram;
cy::GLSLProgram equirectangularToCubemapGLSLProgram;
cy::GLSLProgram irradianceGLSLProgram;
cy::GLSLProgram prefilterGLSLProgram;
cy::GLSLProgram brdfGLSLProgram;
cy::GLSLProgram envMapGLSLProgram;

cy::TriMesh objTriMesh;

GLboolean enablePBR = false;
GLboolean enableTexture = false;
GLboolean enableDiffuseIrradiance = false;
GLboolean enableSpecularIrradiance = false;

GLfloat metallic = 0.0f;
GLfloat roughness = 0.0f;

GLfloat maxMipLevelsForPreFilterMap = 4.0f;

GLuint albedoMapTexID;
GLuint metallicMapTexID;
GLuint roughnessMapTexID;
GLuint aoMapTexID;
GLuint normalMapTexID;

GLuint envCubemap;
GLuint irradianceCubemap;
GLuint prefilterCubemap1;
GLuint prefilterCubemap2;
GLuint prefilterCubemap3;
GLuint prefilterCubemap4;
GLuint prefilterCubemap5;
GLuint brdfTexture;

std::string materialFilePath;
std::string cubeMapFileName;
std::string objectFileName = "resources/objects/";
//buffer objects
GLuint vao;
GLuint vertexbuffer;
GLuint normalbuffer;
GLuint texturebuffer;
GLuint numberOfVertices;
std::vector<cyVec3f> vertexIndices;
std::vector<cyVec3f> normalIndices;
std::vector<cyVec3f> textureIndices;
GLuint cubeVAO;
GLuint cubeVertexBuffer;
GLuint quadVAO;
GLuint quadVertexBuffer;

void initializeObjectBuffer(const char *fileName){
    std::string fileNameString(fileName);
    objTriMesh.LoadFromFileObj(fileName,GL_TRUE);
    objTriMesh.ComputeNormals();
    for(int i=0; i < objTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            vertexIndices.push_back(objTriMesh.V(objTriMesh.F(i).v[j]));
            normalIndices.push_back(objTriMesh.VN(objTriMesh.FN(i).v[j]));
            textureIndices.push_back(objTriMesh.VT(objTriMesh.FT(i).v[j]));
        }
    }
    if(fileNameString.find("teapot.obj") != std::string::npos){
        numberOfVertices = 4257;
    }else{
        numberOfVertices = objTriMesh.NV();
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
    glGenBuffers(1, &texturebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*textureIndices.size(), &textureIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
                          2,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
}

void sendUniformsToObjectShaders(){
    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    glm::mat4 mvMatrix = viewMatrix * modelMatrix;
    
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "model"), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "cameraPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
    
    objGLSLProgram.SetUniform ("enablePBR", enablePBR);
    objGLSLProgram.SetUniform ("enableTexture", enableTexture);
    objGLSLProgram.SetUniform ("enableDiffuseIrradiance", enableDiffuseIrradiance);
    objGLSLProgram.SetUniform ("enableSpecularIrradiance", enableSpecularIrradiance);
    objGLSLProgram.SetUniform ("maxMipLevels", maxMipLevelsForPreFilterMap);
    
    glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "albedoUnifrom"), 0.5f, 0.0f, 0.0f);
    glUniform1f(glGetUniformLocation(objGLSLProgram.GetID(), "aoUnifrom"), 1.0f);
    glUniform1f(glGetUniformLocation(objGLSLProgram.GetID(), "metallicUnifrom"), metallic);
    glUniform1f(glGetUniformLocation(objGLSLProgram.GetID(), "roughnessUnifrom"), glm::clamp(roughness, 0.05f, 1.0f));
}


void renderObject(){
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*numberOfVertices);
    glBindVertexArray(0);
}

void initializeCubeBuffer(){
    
    float vertices[] = {
        // back face
        -10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -10.0f, 0.0f, 0.0f,
        10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -10.0f, 10.0f, 10.0f,
        10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -10.0f, 10.0f, 0.0f,
        10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -10.0f, 10.0f, 10.0f,
        -10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -10.0f, 0.0f, 0.0f,
        -10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -10.0f, 0.0f, 10.0f,
        // front face
        -10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  10.0f, 0.0f, 0.0f,
        10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  10.0f, 10.0f, 0.0f,
        10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  10.0f, 10.0f, 10.0f,
        10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  10.0f, 10.0f, 10.0f,
        -10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  10.0f, 0.0f, 10.0f,
        -10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  10.0f, 0.0f, 0.0f,
        // left face
        -10.0f,  10.0f,  10.0f, -10.0f,  0.0f,  0.0f, 10.0f, 0.0f,
        -10.0f,  10.0f, -10.0f, -10.0f,  0.0f,  0.0f, 10.0f, 10.0f,
        -10.0f, -10.0f, -10.0f, -10.0f,  0.0f,  0.0f, 0.0f, 10.0f,
        -10.0f, -10.0f, -10.0f, -10.0f,  0.0f,  0.0f, 0.0f, 10.0f,
        -10.0f, -10.0f,  10.0f, -10.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -10.0f,  10.0f,  10.0f, -10.0f,  0.0f,  0.0f, 10.0f, 0.0f,
        // right face
        10.0f,  10.0f,  10.0f,  10.0f,  0.0f,  0.0f, 10.0f, 0.0f,
        10.0f, -10.0f, -10.0f,  10.0f,  0.0f,  0.0f, 0.0f, 10.0f,
        10.0f,  10.0f, -10.0f,  10.0f,  0.0f,  0.0f, 10.0f, 10.0f,
        10.0f, -10.0f, -10.0f,  10.0f,  0.0f,  0.0f, 0.0f, 10.0f,
        10.0f,  10.0f,  10.0f,  10.0f,  0.0f,  0.0f, 10.0f, 0.0f,
        10.0f, -10.0f,  10.0f,  10.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        // bottom face
        -10.0f, -10.0f, -10.0f,  0.0f, -10.0f,  0.0f, 0.0f, 10.0f,
        10.0f, -10.0f, -10.0f,  0.0f, -10.0f,  0.0f, 10.0f, 10.0f,
        10.0f, -10.0f,  10.0f,  0.0f, -10.0f,  0.0f, 10.0f, 0.0f,
        10.0f, -10.0f,  10.0f,  0.0f, -10.0f,  0.0f, 10.0f, 0.0f,
        -10.0f, -10.0f,  10.0f,  0.0f, -10.0f,  0.0f, 0.0f, 0.0f,
        -10.0f, -10.0f, -10.0f,  0.0f, -10.0f,  0.0f, 0.0f, 10.0f,
        // top face
        -10.0f,  10.0f, -10.0f,  0.0f,  10.0f,  0.0f, 0.0f, 10.0f,
        10.0f,  10.0f , 10.0f,  0.0f,  10.0f,  0.0f, 10.0f, 0.0f,
        10.0f,  10.0f, -10.0f,  0.0f,  10.0f,  0.0f, 10.0f, 10.0f,
        10.0f,  10.0f,  10.0f,  0.0f,  10.0f,  0.0f, 10.0f, 0.0f,
        -10.0f,  10.0f, -10.0f,  0.0f,  10.0f,  0.0f, 0.0f, 10.0f,
        -10.0f,  10.0f,  10.0f,  0.0f,  10.0f,  0.0f, 0.0f, 0.0f
    };
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    
    glGenBuffers(1, &cubeVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
}

void renderCube(){
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void initializeQuadBuffers(){
    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    
    glGenBuffers(1, &quadVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void renderQuad()
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

GLuint loadHDRImage(std::string fileName){
    stbi_set_flip_vertically_on_load(true);
    int imageWidth;
    int imageHeight;
    int numberOfChannels;
    float *imageData = stbi_loadf(fileName.c_str(), &imageWidth, &imageHeight, &numberOfChannels, 0);
    if (imageData)
    {
        GLuint hdrTexture;
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, imageWidth, imageHeight, 0, GL_RGB, GL_FLOAT, imageData);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return hdrTexture;
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
        return -1;
    }
}

GLuint createEnvMapForSampling(GLuint hdrTexture){
    int textureWidth = 450;
    int textureHeight = 450;
    
    GLuint envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    
    for (GLint i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
    glm::mat4 view[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    
    glViewport(0, 0, textureWidth, textureHeight);
    
    equirectangularToCubemapGLSLProgram.Bind();
    equirectangularToCubemapGLSLProgram.SetUniform("equirectangularMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glUniformMatrix4fv(glGetUniformLocation(equirectangularToCubemapGLSLProgram.GetID(), "projection"), 1, GL_FALSE, &projection[0][0]);
    for (GLint i = 0; i < 6; i++)
    {
        glUniformMatrix4fv(glGetUniformLocation(equirectangularToCubemapGLSLProgram.GetID(), "view"), 1, GL_FALSE, &view[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,envCubemap);
    
    return envCubemap;
}

GLuint createIrradianceMapForSampling(GLuint envTexture){
    GLint textureWidth = 450;
    GLint textureHeight = 450;
    
    GLuint irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    
    for (GLint i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
    GLuint renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, textureWidth, textureHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
    glm::mat4 view[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    
    glViewport(0, 0, textureWidth, textureHeight);
    
    irradianceGLSLProgram.Bind();
    irradianceGLSLProgram.SetUniform("environmentMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envTexture);
    
    glUniformMatrix4fv(glGetUniformLocation(irradianceGLSLProgram.GetID(), "projection"), 1, GL_FALSE, &projection[0][0]);
    for (GLint i = 0; i < 6; i++)
    {
        glUniformMatrix4fv(glGetUniformLocation(irradianceGLSLProgram.GetID(), "view"), 1, GL_FALSE, &view[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return irradianceMap;
}

GLuint createPrefilterMapForSampling(GLuint envTexture, GLint mipLevel = 0){
    GLuint textureWidth = static_cast<GLuint>(125 * std::pow(0.5, mipLevel));
    GLuint textureHeight = static_cast<GLuint>(125 * std::pow(0.5, mipLevel));
    GLuint prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    
    for (GLint i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
    GLuint renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, textureWidth, textureHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
    glm::mat4 view[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    
    glViewport(0, 0, textureWidth, textureHeight);
    
    prefilterGLSLProgram.Bind();
    prefilterGLSLProgram.SetUniform("environmentMap", 0);
    prefilterGLSLProgram.SetUniform("roughness", GLfloat(mipLevel)/maxMipLevelsForPreFilterMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envTexture);
    
    glUniformMatrix4fv(glGetUniformLocation(prefilterGLSLProgram.GetID(), "projection"), 1, GL_FALSE, &projection[0][0]);
    for (GLint i = 0; i < 6; i++)
    {
        glUniformMatrix4fv(glGetUniformLocation(prefilterGLSLProgram.GetID(), "view"), 1, GL_FALSE, &view[i][0][0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mipLevel);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return prefilterMap;
}


GLuint createBRDFTextureForSampling(){
    GLuint textureWidth = 450;
    GLuint textureHeight = 450;
    
    GLuint brdfTexture;
    glGenTextures(1, &brdfTexture);
    glBindTexture(GL_TEXTURE_2D, brdfTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, textureWidth, textureHeight, 0, GL_RG, GL_FLOAT, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
    GLuint renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, textureWidth, textureHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfTexture, 0);
    
    glViewport(0, 0, textureWidth, textureHeight);
    brdfGLSLProgram.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return brdfTexture;
}

GLuint getTexture(std::string fileName){
    GLuint imageWidth = 900;
    GLuint imageHeight = 900;
    std::vector<unsigned char> imageData;
    std::string file(fileName);
    lodepng::decode(imageData, imageWidth, imageHeight, file);
    
    if(imageData.size() > 0){
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageData[0]);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return textureID;
    }else{
        std::cout << "Texture failed to load at path: " << fileName << std::endl;
        return -1;
    }
}


void computeMaterials(){
    if(materialFilePath.empty()){
        materialFilePath = "resources/materials/porcelain-white/";
    }
    std::string albedoMapFileName = materialFilePath;
    albedoMapFileName.append("albedo.png");
    const char *albedoFileName = albedoMapFileName.c_str();
    albedoMapTexID =  getTexture(albedoFileName);
    
    std::string metallicMapFileName = materialFilePath;
    metallicMapFileName.append("metallic.png");
    const char *metallicFileName = metallicMapFileName.c_str();
    metallicMapTexID =  getTexture(metallicFileName);
    
    std::string roughnessMapFileName = materialFilePath;
    roughnessMapFileName.append("roughness.png");
    const char *roughnessFileName = roughnessMapFileName.c_str();
    roughnessMapTexID =  getTexture(roughnessFileName);
    
    std::string aoMapFileName = materialFilePath;
    aoMapFileName.append("ao.png");
    const char *aoFileName = aoMapFileName.c_str();
    aoMapTexID =  getTexture(aoFileName);
    
    std::string normalMapFileName = materialFilePath;
    normalMapFileName.append("normal.png");
    const char *normalFileName = aoMapFileName.c_str();
    normalMapTexID =  getTexture(normalFileName);
}

void computeEnvMap(){
    if(cubeMapFileName.empty()){
        cubeMapFileName = "resources/envmap/hdr/goldroom/Mt-Washington-Gold-Room_Ref.hdr";
    }
    GLuint envHDRTexture = loadHDRImage(cubeMapFileName.c_str());
    envCubemap = createEnvMapForSampling(envHDRTexture);
    //Load irradiance mapping using HDR image
    irradianceCubemap = createIrradianceMapForSampling(envCubemap);
    //Load prefilter mapping using HDR image
    prefilterCubemap1 = createPrefilterMapForSampling(envCubemap, 0);
    prefilterCubemap2 = createPrefilterMapForSampling(envCubemap, 1);
    prefilterCubemap3 = createPrefilterMapForSampling(envCubemap, 2);
    prefilterCubemap4 = createPrefilterMapForSampling(envCubemap, 3);
    prefilterCubemap5 = createPrefilterMapForSampling(envCubemap, 4);
}
#endif /* render_h */
