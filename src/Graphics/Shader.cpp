#include "Graphics/Shader.h"

#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include <glad/gl.h>

#include <fstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace
{
std::string ReadTextFile(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::in | std::ios::binary);
    if (!input)
    {
        throw std::runtime_error("Unable to open shader file: " + path.string());
    }

    input.seekg(0, std::ios::end);
    const auto size = input.tellg();
    if (size < 0)
    {
        throw std::runtime_error("Unable to determine shader file size: " + path.string());
    }

    std::string contents(static_cast<std::size_t>(size), '\0');
    input.seekg(0, std::ios::beg);
    input.read(contents.data(), static_cast<std::streamsize>(contents.size()));
    if (!input && !input.eof())
    {
        throw std::runtime_error("Unable to read shader file: " + path.string());
    }

    return contents;
}

unsigned int CompileShader(
    const unsigned int type,
    const std::string& source,
    const std::filesystem::path& path)
{
    const unsigned int shaderId = glCreateShader(type);
    const char* sourcePointer = source.c_str();
    glShaderSource(shaderId, 1, &sourcePointer, nullptr);
    glCompileShader(shaderId);

    int compiled = GL_FALSE;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE)
    {
        return shaderId;
    }

    int logLength = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> log(static_cast<std::size_t>(logLength > 0 ? logLength : 1), '\0');
    glGetShaderInfoLog(shaderId, logLength, nullptr, log.data());
    glDeleteShader(shaderId);

    throw std::runtime_error(
        "Failed to compile shader '" + path.string() + "':\n" + std::string(log.data()));
}

unsigned int LinkProgram(const unsigned int vertexShader, const unsigned int fragmentShader)
{
    const unsigned int programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);

    int linked = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE)
    {
        return programId;
    }

    int logLength = 0;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> log(static_cast<std::size_t>(logLength > 0 ? logLength : 1), '\0');
    glGetProgramInfoLog(programId, logLength, nullptr, log.data());
    glDeleteProgram(programId);

    throw std::runtime_error("Failed to link shader program:\n" + std::string(log.data()));
}
} // namespace

namespace Forje::Graphics
{
Shader::Shader(
    const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath)
{
    const std::string vertexSource = ReadTextFile(vertexPath);
    const std::string fragmentSource = ReadTextFile(fragmentPath);

    const unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
    unsigned int fragmentShader = 0;

    try
    {
        fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);
        m_ProgramId = LinkProgram(vertexShader, fragmentShader);
    }
    catch (...)
    {
        glDeleteShader(vertexShader);
        if (fragmentShader != 0)
        {
            glDeleteShader(fragmentShader);
        }
        throw;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    if (m_ProgramId != 0)
    {
        glDeleteProgram(m_ProgramId);
    }
}

Shader::Shader(Shader&& other) noexcept
    : m_ProgramId(std::exchange(other.m_ProgramId, 0)),
      m_UniformLocations(std::move(other.m_UniformLocations))
{
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        if (m_ProgramId != 0)
        {
            glDeleteProgram(m_ProgramId);
        }
        m_ProgramId = std::exchange(other.m_ProgramId, 0);
        m_UniformLocations = std::move(other.m_UniformLocations);
    }
    return *this;
}

void Shader::Use() const noexcept
{
    glUseProgram(m_ProgramId);
}

void Shader::SetFloat(const std::string& name, const float value) const
{
    glUniform1f(UniformLocation(name), value);
}

void Shader::SetVector3(const std::string& name, const Math::Vector3& value) const
{
    glUniform3f(UniformLocation(name), value.x, value.y, value.z);
}

void Shader::SetMatrix4(const std::string& name, const Math::Matrix4& value) const
{
    glUniformMatrix4fv(UniformLocation(name), 1, GL_FALSE, value.Data());
}

int Shader::UniformLocation(const std::string& name) const
{
    const auto cached = m_UniformLocations.find(name);
    if (cached != m_UniformLocations.end())
    {
        return cached->second;
    }

    const int location = glGetUniformLocation(m_ProgramId, name.c_str());
    if (location == -1)
    {
        throw std::runtime_error("Shader uniform was not found: " + name);
    }
    m_UniformLocations.emplace(name, location);
    return location;
}
} // namespace Forje::Graphics
