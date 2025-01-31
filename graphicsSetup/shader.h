// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h

#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // get the fragment and vertex code from path
        std::string vCode;
        std::string fCode;
        std::ifstream vShader;
        std::ifstream fShader;
    
        vShader.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShader.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            vShader.open(vertexPath);
            fShader.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
      
            vShaderStream << vShader.rdbuf();
            fShaderStream << fShader.rdbuf();
     
            vShader.close();
            fShader.close();
       
            vCode = vShaderStream.str();
            fCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE READ UNSUCCESSFULL :( : " << e.what() << std::endl;
        }
        const char* vShaderCode = vCode.c_str();
        const char* fShaderCode = fCode.c_str();
        unsigned int vertex, fragment;

        // vertex 
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        errorCheck(vertex, "VERT");
        
        // fragment 
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        errorCheck(fragment, "FRAG");

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        errorCheck(ID, "PROG");
     
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
   
    //function to activate shaders
    void use() const
    {
        glUseProgram(ID);
    }
    // utility functions for uniforms
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
   
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
 
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
 
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
 
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
 
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
 
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
 
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
 
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    
    //Compilation Error Checker
    void errorCheck(GLuint shader, std::string type)
    {
        GLint success;
        GLchar log[1024];
        if (type != "PROG")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, log);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << log << "\n ----------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, log);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << log << "\n ----------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif