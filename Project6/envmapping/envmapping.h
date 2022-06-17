//
//  envmapping.h
//  envmapping
//
//  Created by Santhosh Natarajappa on 3/13/22.
//

#ifndef envmapping_h
#define envmapping_h
cy::TriMesh cubeTriMesh;
cy::GLSLProgram cubeGLSLProgram;
GLuint cube_vao;
std::vector<cyVec3f> cubeVertexIndices;
GLuint cube_vertexBuffer;
GLuint cubeMapTexID;

GLuint getCubMapTextureID(){
    return cubeMapTexID;
}

void loadCubeMapTextures(){
    unsigned imageWidth = screenWidth;
    unsigned imageHeight = screenHeight;
    std::vector<unsigned char> cubeFace_posx;
    lodepng::decode(cubeFace_posx, imageWidth, imageHeight, "resources/cubemap/cubemap_posx.PNG");
    std::vector<unsigned char> cubeFace_negx;
    lodepng::decode(cubeFace_negx, imageWidth, imageHeight, "resources/cubemap/cubemap_negx.PNG");
    std::vector<unsigned char> cubeFace_posy;
    lodepng::decode(cubeFace_posy, imageWidth, imageHeight, "resources/cubemap/cubemap_posy.PNG");
    std::vector<unsigned char> cubeFace_negy;
    lodepng::decode(cubeFace_negy, imageWidth, imageHeight, "resources/cubemap/cubemap_negy.PNG");
    std::vector<unsigned char> cubeFace_posz;
    lodepng::decode(cubeFace_posz, imageWidth, imageHeight, "resources/cubemap/cubemap_posz.PNG");
    std::vector<unsigned char> cubeFace_negz;
    lodepng::decode(cubeFace_negz, imageWidth, imageHeight, "resources/cubemap/cubemap_negz.PNG");
    
    glGenTextures(1, &cubeMapTexID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexID);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubeFace_posx[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubeFace_negx[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubeFace_posy[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubeFace_negy[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubeFace_posz[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubeFace_negz[0]);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void initializeCubeBuffer(const char *fileName){
    //vertex buffers
    cubeTriMesh.LoadFromFileObj(fileName,GL_FALSE);
    
    for(int i=0; i < cubeTriMesh.NF(); i++){
        for(int j=0; j< 3; j++){
            cubeVertexIndices.push_back(cubeTriMesh.V(cubeTriMesh.F(i).v[j]));
        }
    }
    
    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);
    glGenBuffers(1, &cube_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cyVec3f)*cubeVertexIndices.size(), &cubeVertexIndices[0], GL_STATIC_DRAW);
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

void sendUniformsToCubeShaders(){
    glm::mat4 model = modelMatrix;
    glm::mat4 view = viewMatrix;
    glm::mat4 proj = projectionMatrix;
    glm::mat4 mvpMatrix = proj * view * model;
    glUniformMatrix4fv(glGetUniformLocation(cubeGLSLProgram.GetID(), "mvpMatrix"), 1, GL_FALSE, &mvpMatrix[0][0]);
}

void renderCubeMap(){
    glBindVertexArray(cube_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

#endif /* envmapping_h */
