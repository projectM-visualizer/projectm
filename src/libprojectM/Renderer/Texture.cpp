#include "Renderer/Texture.hpp"

#include <utility>

Texture::Texture(std::string name, const int width, const int height, const bool isUserTexture)
    : m_type(GL_TEXTURE_2D)
    , m_name(std::move(name))
    , m_width(width)
    , m_height(height)
    , m_isUserTexture(isUserTexture)
{
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(std::string name, const GLuint texID, const GLenum type,
                 const int width, const int height,
                 const bool isUserTexture)
    : m_textureId(texID)
    , m_type(type)
    , m_name(std::move(name))
    , m_width(width)
    , m_height(height)
    , m_isUserTexture(isUserTexture)
{
}

Texture::~Texture()
{
    if (m_textureId > 0)
    {
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
    }
}

void Texture::Bind(GLint slot, const Sampler::Ptr& sampler) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(m_type, m_textureId);

    if (sampler)
    {
        sampler->Bind(slot);
    }
}

void Texture::Unbind(GLint slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(m_type, 0);
}

auto Texture::TextureID() const -> GLuint
{
    return m_textureId;
}

auto Texture::Name() const -> const std::string&
{
    return m_name;
}

auto Texture::Type() const -> GLenum
{
    return m_type;
}

auto Texture::Width() const -> int
{
    return m_width;
}

auto Texture::Height() const -> int
{
    return m_height;
}

auto Texture::IsUserTexture() const -> bool
{
    return m_isUserTexture;
}
