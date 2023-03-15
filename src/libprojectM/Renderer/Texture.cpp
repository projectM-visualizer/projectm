#include "Renderer/Texture.hpp"

#include <utility>

Texture::Texture(std::string name, const int width, const int height, const bool isUserTexture)
    : m_target(GL_TEXTURE_2D)
    , m_name(std::move(name))
    , m_width(width)
    , m_height(height)
    , m_isUserTexture(isUserTexture)
    , m_internalFormat(GL_RGB)
    , m_format(GL_RGB)
    , m_type(GL_UNSIGNED_BYTE)
{
    CreateEmptyTexture();
}

Texture::Texture(std::string name, int width, int height,
                 GLint internalFormat, GLenum format, GLenum type, bool isUserTexture)
    : m_target(GL_TEXTURE_2D)
    , m_name(std::move(name))
    , m_width(width)
    , m_height(height)
    , m_isUserTexture(isUserTexture)
    , m_internalFormat(internalFormat)
    , m_format(format)
    , m_type(type)
{
    CreateEmptyTexture();
}

Texture::Texture(std::string name, const GLuint texID, const GLenum target,
                 const int width, const int height, const bool isUserTexture)
    : m_textureId(texID)
    , m_target(target)
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
    glBindTexture(m_target, m_textureId);

    if (sampler)
    {
        sampler->Bind(slot);
    }
}

void Texture::Unbind(GLint slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(m_target, 0);
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
    return m_target;
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

void Texture::CreateEmptyTexture()
{
    glGenTextures(1, &m_textureId);
    glBindTexture(m_target, m_textureId);
    glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, nullptr);
    glBindTexture(m_target, 0);
}
