//
//  transformations.h
//  Transformations
//
//  Created by Santhosh Natarajappa.
//

#ifndef transformations_h
#define transformations_h

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

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;


//Initialising the window width and height
const GLint WIDTH=800, HEIGHT=800;
int screenWidth, screenHeight;

GLfloat prevMouseX = 0.0;
GLfloat prevMouseY = 0.0;
GLfloat currMouseX = 0.0;
GLfloat currMouseY = 0.0;

GLfloat sensitivity = 0.2f;

bool firstMouse = true;
//Initialization flags
bool initialized =false;
//Transformation flags
bool rotateCamera =false;
bool zoomCamera = false;

//Light related flags
bool rotateLight =false;
bool controlKeyPressed = false;

glm::vec3 cameraPos = glm::vec3(-0.288f, 0.121f, 4.990f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat fov = 30.0f;

GLfloat horizontalAngle=0.0f;
GLfloat verticalAngle=0.0f;

//Matrix initialization
glm::mat4 modelMatrix = glm::mat4(1.0f);
glm::mat4 viewMatrix = glm::mat4(1.0f);
glm::mat4 projectionMatrix = glm::mat4(1.0f);

//Light Matrices
glm::vec3 lightPosition = glm::vec3(-1.0f, 1.0f, 5.0f);
glm::vec3 lightTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lightUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat lightFOV = 60.0f;
//Matrix initialization
glm::mat4 lightModelMatrix = glm::mat4(1.0f);
glm::mat4 lightViewMatrix = glm::mat4(1.0f);
glm::mat4 lightProjectionMatrix = glm::mat4(1.0f);

cy::GLSLProgram objGLSLProgram;
cy::GLSLProgram stripGLSLProgram;
cy::GLSLProgram planeGLSLProgram;
cy::GLSLProgram shadowGLSLProgram;

cy::TriMesh objTriMesh;
cy::TriMesh shadowTriMesh;

cy::GLRenderDepth2D renderDepth;


//buffer objects
GLuint vao;
GLuint shadow_vao;;
GLuint plane_vao;

GLuint vertexbuffer;
GLuint normalbuffer;

GLuint shadowVertexbuffer;
GLuint shadowNormalbuffer;

GLuint plane_vertexBuffer;

std::vector<cyVec3f> vertexIndices;
std::vector<cyVec3f> normalIndices;

std::vector<cyVec3f> shadowVertexIndices;
std::vector<cyVec3f> shadowNormalIndices;

std::string objectFileName = "resources/objects/";
std::string normalMappingFileName = "resources/mappings/";
std::string dispMappingFileName = "resources/mappings/";


float tessellationLevel = 1.0f;
float displacementLevel = 1.0f;

bool renderStrips = false;
bool renderShadow = false;

void computeMatricesFromInputs(GLFWwindow *window){
    if(!initialized){
        //View
        viewMatrix =glm::lookAt(cameraPos, cameraTarget, cameraUp);
        //Projection
        projectionMatrix = glm::perspective( glm::radians(fov),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
        //Light View
        lightViewMatrix = glm::lookAt(lightPosition, lightTarget, lightUp);
        lightProjectionMatrix = glm::perspective( glm::radians(lightFOV),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
        
        initialized = true;
        
    }else if (currMouseX != prevMouseX || currMouseY != prevMouseY) {
        if(rotateCamera){
            glm::mat4 vRotationMatrix = glm::rotate(glm::radians(verticalAngle), glm::vec3(1, 0, 0));
            glm::mat4 hRotationMatrix = glm::rotate(glm::radians(horizontalAngle), glm::vec3(0, 0, 1));
            cameraPos =hRotationMatrix * glm::vec4(cameraPos,1);
            cameraPos =vRotationMatrix * glm::vec4(cameraPos,1);
            viewMatrix =glm::lookAt(cameraPos, cameraTarget, cameraUp);
            prevMouseX = currMouseX;
            prevMouseY = currMouseY;
        }else if(zoomCamera){
            projectionMatrix = glm::perspective( glm::radians(fov),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
            viewMatrix =glm::lookAt(cameraPos, cameraTarget, cameraUp);
            prevMouseX = currMouseX;
            prevMouseY = currMouseY;
        }else if (rotateLight)
        {
            glm::mat4 vRotationMatrix = glm::rotate(glm::radians(verticalAngle), glm::vec3(1, 0, 0));
            glm::mat4 hRotationMatrix = glm::rotate(glm::radians(-horizontalAngle), glm::vec3(0, 0, 1));
            lightPosition = hRotationMatrix * glm::vec4(lightPosition,1);
            lightPosition = vRotationMatrix * glm::vec4(lightPosition,1);
            lightViewMatrix = glm::lookAt(lightPosition, lightTarget, lightUp);
            prevMouseX = currMouseX;
            prevMouseY = currMouseY;
        }
    }
}

void onKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GL_TRUE);
    }else if (key == GLFW_KEY_F6 && action == GLFW_PRESS){
        objGLSLProgram.BuildFiles("resources/shaders/core.vs", "resources/shaders/core.fs", nullptr, nullptr, nullptr, &std::cout);
    }else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS){
        controlKeyPressed = true;
    }else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        renderStrips = !renderStrips;
    }else if (key == GLFW_KEY_S && action == GLFW_PRESS){
        renderShadow = !renderShadow;
    }else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
        tessellationLevel += 1.0f;
    }else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
        tessellationLevel -= 1.0f;
    }else if (key == GLFW_KEY_UP && action == GLFW_PRESS){
        displacementLevel += 0.1f;
    }else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
        displacementLevel -= 0.1f;
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
        firstMouse = true;
        controlKeyPressed = false;
        rotateLight = false;
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
            if (fov > 40.0f)
                fov = 30.0f;
        }
    }
}

glm::mat4 getModelToWorldMatrix(){
    return modelMatrix;
}


glm::mat4 getWorldToViewMatrix(){
    return viewMatrix;
}



glm::mat4 getProjectionMatrix(){
    return projectionMatrix;
}


glm::mat4 getLightModelMatrix(){
    return lightModelMatrix;
}

glm::mat4 getLightViewMatrix(){
    return lightViewMatrix;
}

glm::mat4 getMVPMatrix(){
    return projectionMatrix*viewMatrix*modelMatrix;
}

glm::mat4 getMVMatrix(){
    return viewMatrix*modelMatrix;
}


glm::mat4 getLightMVMatrix(){
    return lightViewMatrix*lightModelMatrix;
}

glm::mat4 getNormalMVMatrix(){
    return transpose(inverse(viewMatrix*modelMatrix));
}


void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, screenWidth, screenHeight);
}

void checkGLError()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR){
        std::cout << err << std::endl;
    }
}


#endif /* transformations_h */
