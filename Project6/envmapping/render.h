//
//  render.h
//  envmapping
//
//  Created by Santhosh Natarajappa on 3/13/22.
//

#ifndef render_h
#define render_h



void initializeReflectionBuffer(const char *fileName){
    //vertex buffers
    reflectionTriMesh.LoadFromFileObj(fileName,GL_FALSE);
    reflectionTriMesh.ComputeNormals();
    
    for(int i=0; i < reflectionTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            reflectionVertexIndices.push_back(reflectionTriMesh.V(reflectionTriMesh.F(i).v[j]));
            reflectionNormalIndices.push_back(reflectionTriMesh.VN(reflectionTriMesh.FN(i).v[j]));
        }
    }
    
    glGenVertexArrays(1, &reflection_vao);
    glBindVertexArray(reflection_vao);
    
    glGenBuffers(1, &reflectionVertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionVertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*reflectionVertexIndices.size(), &reflectionVertexIndices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    glGenBuffers(1, &reflectionNormalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, reflectionNormalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*reflectionNormalIndices.size(), &reflectionNormalIndices[0], GL_STATIC_DRAW);
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

void sendUniformsToReflectionShaders(){
    //Transformation Logic
    glm::mat4 model = modelMatrix;
    glm::mat4 view = viewMatrix;
    glm::mat4 projection = projectionMatrix;
    model = glm::scale(model, glm::vec3(1,-1,1));
    glm::mat4 mvpMatrix = projection*view*model;
    glm::mat4 mvMatrix = view*model;
    glm::mat4 normalMVMatrix = transpose(inverse(view*model));
    
    glUniformMatrix4fv(glGetUniformLocation(reflectionGLSLProgram.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(reflectionGLSLProgram.GetID(), "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(reflectionGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(reflectionGLSLProgram.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(reflectionGLSLProgram.GetID(), "normalMVMatrix"), 1, GL_FALSE, &normalMVMatrix[0][0]);
    glUniform3f(glGetUniformLocation(reflectionGLSLProgram.GetID(), "cameraPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
    
    //Light arrangement
    glm::mat4 lightMVMatrix = getLightMVMatrix();
    glm::vec3 lightPosition = glm::vec3(100.0f, 50.0f, 0.0f);
    glm::vec3 lightPosition_viewSpace = (lightMVMatrix * vec4(lightPosition, 1.0));
    glUniform3f(glGetUniformLocation(reflectionGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition_viewSpace.x, lightPosition_viewSpace.y, lightPosition_viewSpace.z);
    //Transformation logic end
}

void renderReflection(){
    glBindVertexArray(reflection_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, getCubMapTextureID());
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*reflectionTriMesh.NV());
    glBindVertexArray(0);
}

void initializeTeapotBuffer(const char *fileName){
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

void sendUniformsToTeapotShaders(){
    //Transformation Logic
    glm::mat4 model = modelMatrix;
    glm::mat4 view = viewMatrix;
    glm::mat4 projection = projectionMatrix;
    glm::mat4 mvpMatrix = projection*view*model;
    glm::mat4 mvMatrix = getMVMatrix();
    glm::mat4 normalMVMatrix = getNormalMVMatrix();
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(objGLSLProgram.GetID(), "normalMVMatrix"), 1, GL_FALSE, &normalMVMatrix[0][0]);
    glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "cameraPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
    
    //Light arrangement
    glm::mat4 lightMVMatrix = getLightMVMatrix();
    glm::vec3 lightPosition = glm::vec3(100.0f, 50.0f, 0.0f);
    glm::vec3 lightPosition_viewSpace = (lightMVMatrix * vec4(lightPosition, 1.0));
    glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition_viewSpace.x, lightPosition_viewSpace.y, lightPosition_viewSpace.z);
}

void renderTeapot(){
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexID);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*objTriMesh.NV());
    glBindVertexArray(0);
}


void initializePlaneBuffer(){
    static const GLfloat plane_vertex_buffer_data[] = {
        -1.0f, 0.0f, -1.0f,
        1.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 1.0f,
        -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 1.0f
    };
    glGenVertexArrays(1, &plane_vao);
    glBindVertexArray(plane_vao);
    glGenBuffers(1, &plane_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, plane_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertex_buffer_data), plane_vertex_buffer_data, GL_STATIC_DRAW);
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
void sendUniformsToPlaneShaders(){
    glm::mat4 model = modelMatrix;
    glm::mat4 view = viewMatrix;
    glm::mat4 projection = projectionMatrix;
    glm::mat4 planeMVPMatrix = projection*view*model;
    glm::mat4 mvMatrix = getMVMatrix();
    glm::mat4 normalMVMatrix = getNormalMVMatrix();
    glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "planeMVPMatrix"), 1, GL_FALSE, &planeMVPMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(planeGLSLProgram.GetID(), "normalMVMatrix"), 1, GL_FALSE, &normalMVMatrix[0][0]);
    glUniform3f(glGetUniformLocation(planeGLSLProgram.GetID(), "cameraPosition"), cameraPos.x, cameraPos.y, cameraPos.z);
    
    //Light arrangement
    glm::mat4 lightMVMatrix = getLightMVMatrix();
    glm::vec3 lightPosition = glm::vec3(100.0f, 50.0f, 0.0f);
    glm::vec3 lightPosition_viewSpace = (lightMVMatrix * vec4(lightPosition, 1.0));
    glUniform3f(glGetUniformLocation(planeGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition_viewSpace.x, lightPosition_viewSpace.y, lightPosition_viewSpace.z);
    
    glUniform1i(glGetUniformLocation(planeGLSLProgram.GetID(), "renderedTexture"), 1);
    
}

void renderPlane(){
    glBindVertexArray(plane_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, getCubMapTextureID());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

#endif /* render_h */
