#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace Forje::Math
{
class Matrix4;
struct Vector3;
}

namespace Forje::Graphics
{
class Shader final
{
public:
    Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void Use() const noexcept;
    void SetFloat(const std::string& name, float value) const;
    void SetVector3(const std::string& name, const Math::Vector3& value) const;
    void SetMatrix4(const std::string& name, const Math::Matrix4& value) const;

private:
    int UniformLocation(const std::string& name) const;

    unsigned int m_ProgramId{0};
    mutable std::unordered_map<std::string, int> m_UniformLocations;
};
} // namespace Forje::Graphics
