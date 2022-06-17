//
//  render.h
//  envmapping
//
//  Created by Santhosh Natarajappa on 3/13/22.
//

#ifndef render_h
#define render_h

int screenWidth, screenHeight;

void initializeTeapotBuffer(const char *fileName){
    objTriMesh.LoadFromFileObj(fileName,GL_TRUE);
    objTriMesh.ComputeNormals();
    
    for(int i=0; i < objTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            vertexIndices.push_back(objTriMesh.V(objTriMesh.F(i).v[j]));
            normalIndices.push_back(objTriMesh.VN(objTriMesh.FN(i).v[j]));
            textureIndices.push_back(objTriMesh.VT(objTriMesh.FT(i).v[j]));
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
    
    //Texture buffers
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

void sendUniformsToTeapotShaders(){
    //Transformation Logic
    glm::mat4 model = modelMatrix;
    glm::mat4 view = viewMatrix;
    glm::mat4 projection = projectionMatrix;
    glm::mat4 mvpMatrix = projection * view * model;
    glm::mat4 mvMatrix = view * model;
    glm::mat4 normalMVMatrix = transpose(inverse(mat3(view * model)));
    
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "normalMVMatrix"), 1, GL_FALSE, &normalMVMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition.x, lightPosition.y, lightPosition.z);
    //    std::cout << "light position: " << glm::to_string(lightPosition) << std::endl;

    
    glUniform1i(glGetUniformLocation(objGLSLProgram.GetID(), "shadowMap"), 0);
    glm::mat4 lpMatrix = lightProjectionMatrix * lightViewMatrix;
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "lpMatrix"), 1, GL_FALSE, &lpMatrix[0][0]);
    //Transformation logic end
}


void renderTeapot(){
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*objTriMesh.NV());
    glBindVertexArray(0);
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
    //Transformation Logic
    glm::mat4 model = modelMatrix;
    glm::mat4 lightView = lightViewMatrix;
    glm::mat4 projection = lightProjectionMatrix;
    glm::mat4 lpMatrix = projection * lightView;
    glUniformMatrix4fv(glGetUniformLocation(shadowGLSLProgram.GetID(), "lpMatrix"), 1, GL_FALSE, &lpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shadowGLSLProgram.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    //Transformation logic end
}

void renderTeapotShadow(){
    glBindVertexArray(shadow_vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*shadowTriMesh.NV());
    glBindVertexArray(0);
}


void initializePlaneBuffer(const char *fileName){
    planeTriMesh.LoadFromFileObj(fileName,GL_FALSE);
    planeTriMesh.ComputeNormals();
    
    for(int i=0; i < planeTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            planeVertexIndices.push_back(planeTriMesh.V(planeTriMesh.F(i).v[j]));
            planeNormalIndices.push_back(-1* (planeTriMesh.VN(planeTriMesh.FN(i).v[j])));
            planeTextureIndices.push_back(planeTriMesh.VT(planeTriMesh.FT(i).v[j]));
        }
    }
    
    glGenVertexArrays(1, &plane_vao);
    glBindVertexArray(plane_vao);
    glGenBuffers(1, &planeVertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planeVertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*planeVertexIndices.size(), &planeVertexIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    glGenBuffers(1, &planeNormalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planeNormalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*planeNormalIndices.size(), &planeNormalIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
                          1,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    //Texture buffers
    glGenBuffers(1, &planeTexturebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, planeTexturebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*planeTextureIndices.size(), &planeTextureIndices[0], GL_STATIC_DRAW);
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

void sendUniformsToPlaneShaders(){
    //Transformation Logic
    glm::mat4 model = planeModelMatrix;
    glm::mat4 view = planeViewMatrix;
    glm::mat4 projection = planeProjectionMatrix;
    glm::mat4 mvpMatrix = projection * view * model;
    glm::mat4 mvMatrix = view * model;
    glm::mat4 normalMVMatrix = transpose(inverse(mat3(view * model)));
    
    glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "normalMVMatrix"), 1, GL_FALSE, &normalMVMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(planeGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition.x, lightPosition.y, lightPosition.z);
    std::cout << "light position: " << glm::to_string(lightPosition) << std::endl;

    
    glUniform1i(glGetUniformLocation(planeGLSLProgram.GetID(), "shadowMap"), 0);
    glm::mat4 lpMatrix = lightProjectionMatrix * lightViewMatrix;
    glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "lpMatrix"), 1, GL_FALSE, &lpMatrix[0][0]);
    //Transformation logic end    
}

void renderPlane(){
    glBindVertexArray(plane_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void initializeLightBuffer(const char *fileName){
    lightTriMesh.LoadFromFileObj(fileName,GL_FALSE);
    
    for(int i=0; i < lightTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            light_vertexIndices.push_back(lightTriMesh.V(lightTriMesh.F(i).v[j]));
        }
    }
    
    glGenVertexArrays(1, &light_vao);
    glBindVertexArray(light_vao);
    glGenBuffers(1, &light_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, light_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*light_vertexIndices.size(), &light_vertexIndices[0], GL_STATIC_DRAW);
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

void sendUniformsToLightShaders(){
    //Transformation Logic
    glm::mat4 model = lightObjectModelMatrix;
    glm::mat4 view = lightObjectViewMatrix;
    glm::mat4 projection = projectionMatrix;
    glm::mat4 mvpMatrix = projection * view * model;
    glUniformMatrix4fv(glGetUniformLocation(lightGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
    //Transformation logic end
}

void renderLight(){
    glBindVertexArray(light_vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*lightTriMesh.NV());
    glBindVertexArray(0);
}

#endif /* render_h */
