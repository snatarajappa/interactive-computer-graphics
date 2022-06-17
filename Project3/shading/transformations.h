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


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;

void onKeyAction(GLFWwindow *window, int key, int scancode, int action, int mod);
void onMouseButtonPress(GLFWwindow* window, int button, int action, int mods);
void onMouseMovement(GLFWwindow* window, double xpos, double ypos);
void  computeMatricesFromInputs(GLFWwindow *window);
glm::mat4 getModelToWorldMatrix();
glm::mat4 getWorldToViewMatrix();
glm::mat4 getProjectionMatrix();
glm::mat4 getMVPMatrix();
glm::mat4 getMVMatrix();
glm::mat4 getLightMVMatrix();
glm::mat4 getNormalMVMatrix();
glm::vec3 getArcballVector(int x, int y);

//Initialising the window width and height
const GLint WIDTH=1510, HEIGHT=835;
GLfloat prevMouseX = 0.0;
GLfloat prevMouseY = 0.0;
GLfloat currMouseX = 0.0;
GLfloat currMouseY = 0.0;
GLfloat currCameraDistance = 0.0;
GLfloat prevCameraDistance = 0.0;
glm::vec3 cameraPos = glm::vec3(0.0f, -2.0f, 2.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat horizontalAngle=0.0f;
GLfloat verticalAngle=0.0f;
GLfloat fov = 45.0f;
GLfloat sensitivity = 0.4f;
bool firstMouse = true;
//Initialization flags
bool initialized =false;
//Transformation flags
bool rotateCamera =false;
bool zoomCamera = false;
//Light related flags
bool rotateLight =false;
bool controlKeyPressed = false;


//Matrix initialization
glm::mat4 modelMatrix = glm::mat4(1.0f);
glm::mat4 viewMatrix = glm::mat4(1.0f);
glm::mat4 projectionMatrix = glm::mat4(1.0f);

//Light Matrices
glm::mat4 lightModelMatrix = glm::mat4(1.0f);
glm::mat4 lightViewMatrix = glm::mat4(1.0f);
GLfloat lightRotationAngle=10.0f;
glm::vec3 lightRotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 lightRotationMatrix = glm::mat4(1.0f);

float scaleFactor = 30.0f;

cy::GLSLProgram objGLSLProgram;
cy::TriMesh objTriMesh;


//buffer objects
GLuint vertexbuffer;
GLuint normalbuffer;
GLuint texturebuffer;
std::vector<cyVec3f> vertexIndices;
std::vector<cyVec3f> normalIndices;
std::vector<cyVec3f> textureIndices;

std::string objectFileName = "resources/objects/";



void computeMatricesFromInputs(GLFWwindow *window){
    if(!initialized){
        objTriMesh.ComputeBoundingBox();
        float xBoundValue = (objTriMesh.GetBoundMax().x + objTriMesh.GetBoundMin().x)/2;
        float yBoundValue = (objTriMesh.GetBoundMax().y + objTriMesh.GetBoundMin().y)/2;
        float zBoundValue = (objTriMesh.GetBoundMax().z + objTriMesh.GetBoundMin().z)/2;
        modelMatrix = glm::translate(modelMatrix, 0.05f * glm::vec3(-xBoundValue, -yBoundValue, -zBoundValue));
        std::cout <<"xBoundValue: " << xBoundValue << std::endl;
        std::cout << "yBoundValue: " << yBoundValue << std::endl;
        std::cout <<"zBoundValue: " << zBoundValue << std::endl;
        //View
        viewMatrix =glm::lookAt(cameraPos, cameraTarget, cameraUp);
        //Projection
        projectionMatrix = glm::perspective( glm::radians(fov),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
        
        //Light Model
        lightModelMatrix = glm::mat4(scaleFactor);
        //Light View
        lightViewMatrix = glm::lookAt(glm::vec3(70.0f, 50.0f ,0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        initialized = true;
    }else if (currMouseX != prevMouseX || currMouseY != prevMouseY) {
        if(rotateCamera){
            viewMatrix = glm::rotate(viewMatrix, glm::radians(verticalAngle), glm::vec3(1, 0, 0));
            viewMatrix = glm::rotate(viewMatrix, glm::radians(horizontalAngle), glm::vec3(0, 0, 1));
            prevMouseX = currMouseX;
            prevMouseY = currMouseY;
        }else if (rotateLight)
        {
            lightViewMatrix = glm::rotate(lightViewMatrix, glm::radians(-horizontalAngle), glm::vec3(1, 0, 0));
            lightViewMatrix = glm::rotate(lightViewMatrix, glm::radians(-verticalAngle), glm::vec3(0, 0, 1));
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
    }else if (key == GLFW_KEY_F6 && action == GLFW_PRESS){
        objGLSLProgram.BuildFiles("resources/shaders/core.vs", "resources/shaders/core.fs", nullptr, nullptr, nullptr, &std::cout);
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
        fov += (float)yoffset;
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 100.0f)
            fov = 100.0f;
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
    return transpose(inverse(mat3(viewMatrix*modelMatrix)));
}

#endif /* transformations_h */

