#include "VertexIndexArray.hpp"

namespace libprojectM {
namespace Renderer {

VertexIndexArray::VertexIndexArray()
{
    glGenBuffers(1, &m_veabID);
}

VertexIndexArray::VertexIndexArray(VertexBufferUsage usage)
    : m_vboUsage(usage)
{
    glGenBuffers(1, &m_veabID);
}

VertexIndexArray::~VertexIndexArray()
{
    glDeleteBuffers(1, &m_veabID);
}

void VertexIndexArray::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_veabID);
}

void VertexIndexArray::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

auto VertexIndexArray::Get() -> std::vector<uint32_t>&
{
    return m_indices;
}

auto VertexIndexArray::Get() const -> const std::vector<uint32_t>&
{
    return m_indices;
}

void VertexIndexArray::Set(const std::vector<uint32_t>& buffer)
{
    m_indices = buffer;
}

auto VertexIndexArray::VertexIndex(size_t index) -> uint32_t
{
    return m_indices.at(index);
}

auto VertexIndexArray::VertexIndex(size_t index) const -> uint32_t
{
    return m_indices.at(index);
}

void VertexIndexArray::SetVertexIndex(size_t index, uint32_t value)
{
    m_indices.at(index) = value;
}

auto VertexIndexArray::Empty() const -> bool
{
    return m_indices.empty();
}

auto VertexIndexArray::Size() const -> size_t
{
    return m_indices.size();
}

void VertexIndexArray::Resize(size_t size)
{
    m_indices.resize(size);
}

void VertexIndexArray::Resize(size_t size, uint32_t value)
{
    m_indices.resize(size, value);
}

void VertexIndexArray::MakeContinuous()
{
    for (size_t index = 0; index < m_indices.size(); index++)
    {
        m_indices.at(index) = index;
    }
}

void VertexIndexArray::Update()
{
    Bind();

    if (m_indices.empty())
    {
        return;
    }

    if (m_veabSize == m_indices.size())
    {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<GLsizei>(sizeof(uint32_t) * m_indices.size()), m_indices.data());
    }
    else
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(uint32_t) * m_indices.size()), m_indices.data(), VertexBufferUsageToGL(m_vboUsage));
        m_veabSize = m_indices.size();
    }
}

auto VertexIndexArray::operator[](size_t index) -> uint32_t&
{
    return m_indices.at(index);
}

auto VertexIndexArray::operator[](size_t index) const -> uint32_t
{
    return m_indices.at(index);
}

} // namespace Renderer
} // namespace libprojectM
