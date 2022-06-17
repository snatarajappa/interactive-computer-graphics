//
//  render.h
//  envmapping
//
//  Created by Santhosh Natarajappa on 3/13/22.
//

#ifndef render_h
#define render_h

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
    glm::vec3 lightPosition_viewSpace = (lightMVMatrix * vec4(lightPosition, 1.0));
    glUniform3f(glGetUniformLocation(objGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition_viewSpace.x, lightPosition_viewSpace.y, lightPosition_viewSpace.z);
}

void renderTeapot(){
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cyVec3f)*objTriMesh.NV());
    glBindVertexArray(0);
}


void initializeObjectBuffer(){
    static const GLfloat plane_vertex_buffer_data[] = {
        -1.0f, 0.0f, -1.0f, (1)
        1.0f, 0.0f, -1.0f,  (2)
        -1.0f, 0.0f, 1.0f,  (3)
        -1.0f, 0.0f, 1.0f,  (1)
        1.0f, 0.0f, -1.0f,  (3)
        1.0f, 0.0f, 1.0f    (4)
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
    glm::vec3 lightPosition_viewSpace = (lightMVMatrix * vec4(lightPosition, 1.0));
    glUniform3f(glGetUniformLocation(planeGLSLProgram.GetID(), "lightPosition_viewSpace"), lightPosition_viewSpace.x, lightPosition_viewSpace.y, lightPosition_viewSpace.z);
    
    glUniform1i(glGetUniformLocation(planeGLSLProgram.GetID(), "depthMap"), 0);
    
//    std::cout << "Light position: " << glm::to_string(lightPosition_viewSpace) << std::endl;
    
}

void renderPlane(){
    glBindVertexArray(plane_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

#endif /* render_h */
