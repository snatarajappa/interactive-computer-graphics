//Fragment shader for cube
#version 330 core

layout(location = 0) out vec4 color;

in vec3 textureDir;

uniform samplerCube skybox;

void main()
{
    color = texture(skybox, textureDir);
}
