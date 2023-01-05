#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragPos = vec3(model * vec4(vPos, 1.0));
    normal = mat3(transpose(inverse(model))) * vNormal;  

    texCoord = vTexCoord;    
    gl_Position = projection * view * model * vec4(vPos, 1.0);
}