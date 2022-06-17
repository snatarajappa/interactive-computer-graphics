//
//  envmapping.h
//  envmapping
//
//  Created by Santhosh Natarajappa on 3/13/22.
//

#ifndef envmapping_h
#define envmapping_h
cy::TriMesh shadowTriMesh;
cy::GLSLProgram shadowGLSLProgram;
GLuint shadow_vao;
std::vector<cyVec3f> shadowVertexIndices;
GLuint shadow_vertexBuffer;

GLuint FramebufferName = 0;
GLuint depthTexture;

void initializeShadowBuffer(const char *fileName){
    //vertex buffers
    shadowTriMesh.LoadFromFileObj(fileName,GL_FALSE);
    
    for(int i=0; i < shadowTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            shadowVertexIndices.push_back(shadowTriMesh.V(shadowTriMesh.F(i).v[j]));
        }
    }
    
    glGenVertexArrays(1, &shadow_vao);
    glBindVertexArray(shadow_vao);
    glGenBuffers(1, &shadow_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, shadow_vertexBuffer);
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
}

void sendUniformsToShadowShaders(){
    // Compute the MVP matrix from the light's point of view
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
    glm::mat4 depthViewMatrix = glm::lookAt(lightPosition, glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 depthModelMatrix = glm::mat4(1.0);
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
    glUniformMatrix4fv(glGetUniformLocation(shadowGLSLProgram.GetID(), "depthMVP"), 1, GL_FALSE, &depthMVP[0][0]);
}

void renderDepthTexture(){
    glBindVertexArray(shadow_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

bool initializeDepthFrameBuffer(){
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, screenWidth, screenHeight, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
    
    glDrawBuffer(GL_NONE); // No color buffer is drawn to.
    glReadBuffer(GL_NONE);
    
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;
    
    return true;
}

#endif /* envmapping_h */
