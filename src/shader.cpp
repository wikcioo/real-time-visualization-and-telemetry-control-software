#include "../include/shader.hpp"

#include <fstream>
#include <GL/glew.h>

#define IS_SHADER_TYPE(type) ((type) == 0x8B31 || (type) == 0x8B30)

Shader::Shader(const std::string& vertex_shader_filename, const std::string& fragment_shader_filename)
    : m_ProgramID(0)
{
    CreateProgramFromFiles(vertex_shader_filename, fragment_shader_filename);
}

Shader::~Shader()
{
    glDeleteProgram(m_ProgramID);
}

void Shader::Bind()
{
    glUseProgram(m_ProgramID);
}

void Shader::Unbind()
{
    glUseProgram(0);
}

void Shader::SetInt(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(m_ProgramID, name.c_str()), value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& vec3)
{
    glUniform3fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, &vec3[0]);
}

void Shader::SetVec4(const std::string& name, const glm::vec4& vec4)
{
    glUniform4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, &vec4[0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat4)
{
    glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, &mat4[0][0]);
}

void Shader::HotReloadProgram(const std::string& new_vertex_shader_filename, const std::string& new_fragment_shader_filename)
{
    glDeleteProgram(m_ProgramID);
    CreateProgramFromFiles(new_vertex_shader_filename, new_fragment_shader_filename);
}

void Shader::CreateProgramFromFiles(const std::string& vertex_shader_filename, const std::string& fragment_shader_filename)
{
    std::ifstream vs_ifs(vertex_shader_filename);
    const std::string vs_content((std::istreambuf_iterator<char>(vs_ifs)), (std::istreambuf_iterator<char>()));

    std::ifstream fs_ifs(fragment_shader_filename);
    const std::string fs_content((std::istreambuf_iterator<char>(fs_ifs)), (std::istreambuf_iterator<char>()));

    CreateProgramFromStrings(vs_content, fs_content);
}

void Shader::CreateProgramFromStrings(const std::string& vertex_shader_str, const std::string& fragment_shader_str)
{
    m_ProgramID = glCreateProgram();

    unsigned int vs = CompileShader(vertex_shader_str, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragment_shader_str, GL_FRAGMENT_SHADER);

    glAttachShader(m_ProgramID, vs);
    glAttachShader(m_ProgramID, fs);

    glLinkProgram(m_ProgramID);
    CheckForShaderErrors(m_ProgramID, "Program");

    glDeleteShader(vs);
    glDeleteShader(fs);
}

unsigned int Shader::CompileShader(const std::string& source, unsigned int type)
{
    assert(IS_SHADER_TYPE(type));

    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    CheckForShaderErrors(shader, type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
    return shader;
}

void Shader::CheckForShaderErrors(unsigned int id, const std::string& name)
{
    int status;
    char errorLog[1024];

    if (name == "Program")
    {
        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if (!status)
        {
            glGetProgramInfoLog(id, 1024, nullptr, errorLog);
            fprintf(stderr, "Shader program error: '%s'\n", errorLog);
        }
    }
    else
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            glGetShaderInfoLog(id, 1024, nullptr, errorLog);
            fprintf(stderr, "%s Shader error: '%s'\n", name.c_str(), errorLog);
        }
    }
}
