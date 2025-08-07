#pragma once

#include "Renderer/VertexBufferUsage.hpp"

#include <projectM-opengl.h>

#include <cstddef>
#include <vector>

namespace libprojectM {
namespace Renderer {

template<class VT>
class VertexBuffer
{
public:
    VertexBuffer();

    explicit VertexBuffer(VertexBufferUsage usage);

    virtual ~VertexBuffer();

    void Bind() const;

    static void Unbind();

    void InitializeAttributePointer(uint32_t attributeIndex) const;

    static void SetEnableAttributeArray(uint32_t attributeIndex, bool enable);

    auto Get() -> std::vector<VT>&;

    auto Get() const -> const std::vector<VT>&;

    void Set(const std::vector<VT>& buffer);

    auto Vertex(size_t index) -> VT&;

    auto Vertex(size_t index) const -> const VT&;

    void SetVertex(size_t index, const VT& vertex);

    auto Empty() const -> bool;

    auto Size() const -> size_t;

    void Resize(size_t size);

    void Resize(size_t size, const VT& elem);


    void Update();

    auto operator[](size_t index) -> VT&;

    auto operator[](size_t index) const -> const VT&;

private:
    GLuint m_vboID{};
    size_t m_vboSize{};

    VertexBufferUsage m_vboUsage{VertexBufferUsage::StaticDraw};

    std::vector<VT> m_vertices;
};

template<class VT>
VertexBuffer<VT>::VertexBuffer()
{
    glGenBuffers(1, &m_vboID);
}

template<class VT>
VertexBuffer<VT>::VertexBuffer(VertexBufferUsage usage)
    : m_vboUsage(usage)
{
    glGenBuffers(1, &m_vboID);
}

template<class VT>
VertexBuffer<VT>::~VertexBuffer()
{
    glDeleteBuffers(1, &m_vboID);
}

template<class VT>
void VertexBuffer<VT>::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
}

template<class VT>
void VertexBuffer<VT>::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<class VT>
void VertexBuffer<VT>::InitializeAttributePointer(uint32_t attributeIndex) const
{
    Bind();
    VT::InitializeAttributePointer(attributeIndex);
}

template<class VT>
void VertexBuffer<VT>::SetEnableAttributeArray(uint32_t attributeIndex, bool enable)
{
    if (enable)
    {
        glEnableVertexAttribArray(attributeIndex);
    }
    else
    {
        glDisableVertexAttribArray(attributeIndex);
    }
}

template<class VT>
auto VertexBuffer<VT>::Get() -> std::vector<VT>&
{
    return m_vertices;
}

template<class VT>
auto VertexBuffer<VT>::Get() const -> const std::vector<VT>&
{
    return m_vertices;
}

template<class VT>
void VertexBuffer<VT>::Set(const std::vector<VT>& buffer)
{
    m_vertices = buffer;
}

template<class VT>
auto VertexBuffer<VT>::Vertex(size_t index) -> VT&
{
    return m_vertices.at(index);
}

template<class VT>
auto VertexBuffer<VT>::Vertex(size_t index) const -> const VT&
{
    return m_vertices.at(index);
}

template<class VT>
void VertexBuffer<VT>::SetVertex(size_t index, const VT& vertex)
{
    m_vertices.at(index) = vertex;
}

template<class VT>
auto VertexBuffer<VT>::Empty() const -> bool
{
    return m_vertices.empty();
}

template<class VT>
auto VertexBuffer<VT>::Size() const -> size_t
{
    return m_vertices.size();
}

template<class VT>
void VertexBuffer<VT>::Resize(size_t size)
{
    m_vertices.resize(size);
}

template<class VT>
void VertexBuffer<VT>::Resize(size_t size, const VT& elem)
{
    m_vertices.resize(size, elem);
}

template<class VT>
void VertexBuffer<VT>::Update()
{
    Bind();

    if (m_vertices.empty())
    {
        return;
    }

    if (m_vboSize == m_vertices.size())
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizei>(sizeof(VT) * m_vertices.size()), m_vertices.data());
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(VT) * m_vertices.size()), m_vertices.data(), VertexBufferUsageToGL(m_vboUsage));
        m_vboSize = m_vertices.size();
    }
}

template<class VT>
auto VertexBuffer<VT>::operator[](size_t index) const -> const VT&
{
    return m_vertices.at(index);
}

template<class VT>
auto VertexBuffer<VT>::operator[](size_t index) -> VT&
{
    return m_vertices.at(index);
}

} // namespace Renderer
} // namespace libprojectM
