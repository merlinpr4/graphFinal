#version 330 core
out vec4 fragColour;
in vec3 texCoord;
uniform samplerCube skybox;

void main()
{    
    fragColour = texture(skybox, texCoord);
}