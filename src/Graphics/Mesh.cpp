#include "Graphics/Mesh.h"

#include <glad/gl.h>

#include <cstddef>
#include <limits>
#include <stdexcept>

namespace Forje::Graphics
{
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
{
    if (vertices.empty() || indices.empty())
    {
        throw std::invalid_argument("A mesh requires vertices and indices.");
    }
    if (indices.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        throw std::overflow_error("Mesh index count exceeds the OpenGL draw limit.");
    }

    m_IndexCount = static_cast<int>(indices.size());
    glGenVertexArrays(1, &m_VertexArray);
    glGenBuffers(1, &m_VertexBuffer);
    glGenBuffers(1, &m_IndexBuffer);

    glBindVertexArray(m_VertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(std::uint32_t)),
        indices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        static_cast<int>(sizeof(Vertex)),
        reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        static_cast<int>(sizeof(Vertex)),
        reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::~Mesh()
{
    if (m_IndexBuffer != 0)
    {
        glDeleteBuffers(1, &m_IndexBuffer);
    }
    if (m_VertexBuffer != 0)
    {
        glDeleteBuffers(1, &m_VertexBuffer);
    }
    if (m_VertexArray != 0)
    {
        glDeleteVertexArrays(1, &m_VertexArray);
    }
}

void Mesh::Draw() const noexcept
{
    glBindVertexArray(m_VertexArray);
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
}

std::vector<Vertex> Mesh::CubeVertices()
{
    return {
        {{-0.5F, -0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}},
        {{0.5F, -0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}},
        {{0.5F, 0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}},
        {{-0.5F, 0.5F, 0.5F}, {0.0F, 0.0F, 1.0F}},

        {{0.5F, -0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}},
        {{-0.5F, -0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}},
        {{-0.5F, 0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}},
        {{0.5F, 0.5F, -0.5F}, {0.0F, 0.0F, -1.0F}},

        {{-0.5F, -0.5F, -0.5F}, {-1.0F, 0.0F, 0.0F}},
        {{-0.5F, -0.5F, 0.5F}, {-1.0F, 0.0F, 0.0F}},
        {{-0.5F, 0.5F, 0.5F}, {-1.0F, 0.0F, 0.0F}},
        {{-0.5F, 0.5F, -0.5F}, {-1.0F, 0.0F, 0.0F}},

        {{0.5F, -0.5F, 0.5F}, {1.0F, 0.0F, 0.0F}},
        {{0.5F, -0.5F, -0.5F}, {1.0F, 0.0F, 0.0F}},
        {{0.5F, 0.5F, -0.5F}, {1.0F, 0.0F, 0.0F}},
        {{0.5F, 0.5F, 0.5F}, {1.0F, 0.0F, 0.0F}},

        {{-0.5F, 0.5F, 0.5F}, {0.0F, 1.0F, 0.0F}},
        {{0.5F, 0.5F, 0.5F}, {0.0F, 1.0F, 0.0F}},
        {{0.5F, 0.5F, -0.5F}, {0.0F, 1.0F, 0.0F}},
        {{-0.5F, 0.5F, -0.5F}, {0.0F, 1.0F, 0.0F}},

        {{-0.5F, -0.5F, -0.5F}, {0.0F, -1.0F, 0.0F}},
        {{0.5F, -0.5F, -0.5F}, {0.0F, -1.0F, 0.0F}},
        {{0.5F, -0.5F, 0.5F}, {0.0F, -1.0F, 0.0F}},
        {{-0.5F, -0.5F, 0.5F}, {0.0F, -1.0F, 0.0F}},
    };
}

std::vector<std::uint32_t> Mesh::CubeIndices()
{
    return {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20,
    };
}
} // namespace Forje::Graphics
