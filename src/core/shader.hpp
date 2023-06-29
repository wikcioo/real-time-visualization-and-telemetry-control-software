#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const std::string& vertex_shader_filename, const std::string& fragment_shader_filename);
    ~Shader();

    void Bind();
    void Unbind();

    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec3(const std::string& name, const glm::vec3& vec3);
    void SetVec4(const std::string& name, const glm::vec4& vec4);
    void SetMat4(const std::string& name, const glm::mat4& mat4);

    void HotReloadProgram(const std::string& new_vertex_shader_filename, const std::string& new_fragment_shader_filename);

    inline unsigned int GetProgramID() { return m_ProgramID; }
private:
    unsigned int m_ProgramID;

    void CreateProgramFromFiles(const std::string& vertex_shader_filename, const std::string& fragment_shader_filename);
    void CreateProgramFromStrings(const std::string& vertex_shader_str, const std::string& fragment_shader_str);
    unsigned int CompileShader(const std::string& source, unsigned int type);
    void CheckForShaderErrors(unsigned int id, const std::string& name);
};
