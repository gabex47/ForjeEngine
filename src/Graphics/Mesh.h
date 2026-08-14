#pragma once

#include "Math/Vector3.h"

#include <cstdint>
#include <vector>

namespace Forje::Graphics
{
struct Vertex
{
    Math::Vector3 position;
    Math::Vector3 normal;
};

class Mesh final
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    void Draw() const noexcept;

    static std::vector<Vertex> CubeVertices();
    static std::vector<std::uint32_t> CubeIndices();

private:
    unsigned int m_VertexArray{0};
    unsigned int m_VertexBuffer{0};
    unsigned int m_IndexBuffer{0};
    int m_IndexCount{0};
};
} // namespace Forje::Graphics
