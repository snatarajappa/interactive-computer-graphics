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
const GLint WIDTH=1510, HEIGHT=835;

GLfloat prevMouseX = 0.0;
GLfloat prevMouseY = 0.0;
GLfloat currMouseX = 0.0;
GLfloat currMouseY = 0.0;
GLfloat currCameraDistance = 0.0;
GLfloat prevCameraDistance = 0.0;
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
//Plane related
bool altKeyPressed = false;
bool rotatePlane = false;
bool zoomPlane = false;

GLfloat horizontalAngle=0.0f;
GLfloat verticalAngle=0.0f;

glm::vec3 planeCameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 planeCameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 planeCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat planeFOV = 60.0f;
//Plane Matrices
glm::mat4 planeModelMatrix = glm::mat4(1.0f);
glm::mat4 planeViewMatrix = glm::mat4(1.0f);
glm::mat4 planeProjectionMatrix = glm::mat4(1.0f);



glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat fov = 60.0f;
//Matrix initialization
glm::mat4 modelMatrix = glm::mat4(1.0f);
glm::mat4 viewMatrix = glm::mat4(1.0f);
glm::mat4 projectionMatrix = glm::mat4(1.0f);



//Light Matrices
glm::vec3 lightPosition = glm::vec3(-3.0f, 1.0f, 3.0f);
glm::vec3 lightTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lightUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat lightFOV = 60.0f;
//Matrix initialization
glm::mat4 lightModelMatrix = glm::mat4(1.0f);
glm::mat4 lightViewMatrix = glm::mat4(1.0f);
glm::mat4 lightProjectionMatrix = glm::mat4(1.0f);

glm::mat4 lightObjectModelMatrix = glm::mat4(1.0f);
glm::mat4 lightObjectViewMatrix = glm::mat4(1.0f);


GLfloat lightRotationAngle=0.0f;
glm::vec3 lightRotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 lightRotationMatrix = glm::mat4(1.0f);

cy::GLSLProgram objGLSLProgram;
cy::GLSLProgram planeGLSLProgram;
cy::GLSLProgram shadowGLSLProgram;
cy::GLSLProgram lightGLSLProgram;

cy::TriMesh objTriMesh;
cy::TriMesh shadowTriMesh;
cy::TriMesh planeTriMesh;
cy::TriMesh lightTriMesh;

cy::GLRenderDepth2D renderDepth;


//buffer objects
GLuint vao;
GLuint plane_vao;
GLuint shadow_vao;
GLuint light_vao;

GLuint vertexbuffer;
GLuint normalbuffer;
GLuint texturebuffer;

GLuint shadowVertexbuffer;
GLuint shadowNormalbuffer;

GLuint planeVertexbuffer;
GLuint planeNormalbuffer;
GLuint planeTexturebuffer;

GLuint light_vertexbuffer;

std::vector<cyVec3f> vertexIndices;
std::vector<cyVec3f> normalIndices;
std::vector<cyVec3f> textureIndices;

std::vector<cyVec3f> shadowVertexIndices;
std::vector<cyVec3f> shadowNormalIndices;

std::vector<cyVec3f> planeVertexIndices;
std::vector<cyVec3f> planeNormalIndices;
std::vector<cyVec3f> planeTextureIndices;

std::vector<cyVec3f> light_vertexIndices;

std::string objectFileName = "resources/objects/";



void computeMatricesFromInputs(GLFWwindow *window){
    if(!initialized){
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, 0.05, 0.05));
        viewMatrix =glm::lookAt(cameraPos, cameraTarget, cameraUp);
        projectionMatrix = glm::perspective( glm::radians(fov),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
        
        lightObjectModelMatrix = glm::scale(lightObjectModelMatrix, glm::vec3(0.01, 0.01, 0.01));
        lightObjectViewMatrix = viewMatrix;
        lightObjectViewMatrix = glm::translate(lightObjectViewMatrix, lightPosition);
        //Light View
        lightViewMatrix = glm::lookAt(lightPosition, lightTarget, lightUp);
        lightProjectionMatrix = glm::perspective( glm::radians(lightFOV),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
        
        //Plane
        planeViewMatrix =glm::lookAt(planeCameraPos, planeCameraTarget, planeCameraUp);
        planeProjectionMatrix = glm::perspective( glm::radians(planeFOV),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
        
        initialized = true;
    }else if (currMouseX != prevMouseX || currMouseY != prevMouseY) {
        if(rotatePlane){
            //            planeViewMatrix = glm::rotate(planeViewMatrix, glm::radians(planeVerticalAngle), glm::vec3(1, 0, 0));
            //            planeViewMatrix = glm::rotate(planeViewMatrix, glm::radians(planeHorizontalAngle), glm::vec3(0, 1, 0));
            //            prevMouseX = currMouseX;
            //            prevMouseY = currMouseY;
        }else if(zoomPlane){
            //            planeProjectionMatrix = glm::perspective( glm::radians(planeFOV),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
            //            prevMouseX = currMouseX;
            //            prevMouseY = currMouseY;
        }else if(rotateCamera){
            glm::mat4 vRotationMatrix = glm::rotate(glm::radians(verticalAngle), glm::vec3(1, 0, 0));
            glm::mat4 hRotationMatrix = glm::rotate(glm::radians(-horizontalAngle), glm::vec3(0, 1, 0));
            cameraPos =hRotationMatrix * glm::vec4(cameraPos,1);
            planeCameraPos =hRotationMatrix * glm::vec4(planeCameraPos,1);
            cameraPos =vRotationMatrix * glm::vec4(cameraPos,1);
            planeCameraPos =vRotationMatrix * glm::vec4(planeCameraPos,1);
            viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
            lightObjectViewMatrix = viewMatrix;
            lightObjectViewMatrix = glm::translate(lightObjectViewMatrix, lightPosition);
            planeViewMatrix = glm::lookAt(planeCameraPos, planeCameraTarget, planeCameraUp);
            prevMouseX = currMouseX;
            prevMouseY = currMouseY;
        }else if (rotateLight)
        {
            glm::mat4 vRotationMatrix = glm::rotate(glm::radians(verticalAngle), glm::vec3(1, 0, 0));
            glm::mat4 hRotationMatrix = glm::rotate(glm::radians(-horizontalAngle), glm::vec3(0, 1, 0));
            lightPosition = hRotationMatrix * glm::vec4(lightPosition,1);
            lightPosition = vRotationMatrix * glm::vec4(lightPosition,1);
            lightViewMatrix = glm::lookAt(lightPosition, lightTarget, lightUp);
            lightObjectViewMatrix = viewMatrix;
            lightObjectViewMatrix = glm::translate(lightObjectViewMatrix, lightPosition);
            prevMouseX = currMouseX;
            prevMouseY = currMouseY;
        }else if(zoomCamera){
            projectionMatrix = glm::perspective( glm::radians(fov),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
            planeProjectionMatrix = glm::perspective( glm::radians(planeFOV),(float)WIDTH / (float)HEIGHT,0.1f,1000.0f);
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
    }else if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS){
        altKeyPressed = true;
    }else if (key == GLFW_KEY_F6 && action == GLFW_PRESS){
        objGLSLProgram.BuildFiles("resources/shaders/teapot.vs", "resources/shaders/teapot.fs", nullptr, nullptr, nullptr, &std::cout);
        planeGLSLProgram.BuildFiles("resources/shaders/plane.vs", "resources/shaders/plane.fs", nullptr, nullptr, nullptr, &std::cout);
        shadowGLSLProgram.BuildFiles("resources/shaders/shadow.vs", "resources/shaders/shadow.fs", nullptr, nullptr, nullptr, &std::cout);
        lightGLSLProgram.BuildFiles("resources/shaders/light.vs", "resources/shaders/light.fs", nullptr, nullptr, nullptr, &std::cout);
    }
}

void onMouseButtonPress(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if(controlKeyPressed){
            rotateLight = true;
        }else if(altKeyPressed){
            rotatePlane = true;
        }else{
            rotateCamera = true;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        if(altKeyPressed){
            zoomPlane = true;
        }else{
            zoomCamera = true;
        }
    }
    if (action == GLFW_RELEASE)
    {
        rotateCamera = false;
        zoomCamera = false;
        controlKeyPressed = false;
        rotateLight = false;
        altKeyPressed = false;
        rotatePlane = false;
        zoomPlane = false;
        firstMouse = true;
    }
}

void onMouseMovement(GLFWwindow* window, double xPos, double yPos)
{
    if (rotateCamera || zoomCamera || rotateLight || rotatePlane || zoomPlane)
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
            
            planeFOV += (float)yoffset;
            if (planeFOV < 1.0f)
                planeFOV = 1.0f;
            if (planeFOV > 100.0f)
                planeFOV = 100.0f;
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

glm::mat4 getPlaneMVPMatrix(){
    return planeProjectionMatrix * planeViewMatrix * planeModelMatrix;
}

//glm::mat4 getLightCameraMVPMatrix(){
//    return lightCameraProjectionMatrix*lightCameraViewMatrix*lightCameraModelMatrix;
//}


//glm::mat4 getShadowMatrix(){
//    return  glm::translate(glm::scale(getLightCameraMVPMatrix(), glm::vec3(0.5,0.5,0.5)), glm::vec3(0.5,0.5,0.5));
//}

glm::mat4 getMVMatrix(){
    return viewMatrix*modelMatrix;
}

glm::mat4 getLightMVMatrix(){
    return lightViewMatrix*lightModelMatrix;
}

//glm::mat4 getLightCameraMVMatrix(){
//    return lightCameraViewMatrix*lightCameraModelMatrix;
//}

glm::mat4 getNormalMVMatrix(){
    return transpose(inverse(mat3(viewMatrix*modelMatrix)));
}

void checkGLError()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR){
        std::cout << err << std::endl;
    }
}

#endif /* transformations_h */
