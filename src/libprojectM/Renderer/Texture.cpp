#include "Renderer/Texture.hpp"

#include <utility>

namespace libprojectM {
namespace Renderer {

Texture::Texture(std::string name, const int width, const int height, const enum Source source)
    : m_target(GL_TEXTURE_2D)
    , m_name(std::move(name))
    , m_width(width)
    , m_height(height)
    , m_source(source)
    , m_internalFormat(GL_RGB)
    , m_format(GL_RGB)
    , m_type(GL_UNSIGNED_BYTE)
{
    CreateNewTexture();
}

Texture::Texture(std::string name, GLenum target, int width, int height, int depth,
                 GLint internalFormat, GLenum format, GLenum type, const enum Source source)
    : m_target(target)
    , m_name(std::move(name))
    , m_width(width)
    , m_height(height)
    , m_depth(depth)
    , m_source(source)
    , m_internalFormat(internalFormat)
    , m_format(format)
    , m_type(type)
{
    CreateNewTexture();
}

Texture::Texture(std::string name, const GLuint texID, const GLenum target,
                 const int width, const int height, const enum Source source)
    : m_textureId(texID)
    , m_target(target)
    , m_name(std::move(name))
    , m_width(width)
    , m_height(height)
    , m_source(source)
{
}

Texture::Texture(std::string name, const void* data, GLenum target, int width, int height, int depth,
                 GLint internalFormat, GLenum format, GLenum type, const enum Source source)
    : m_target(target)
    , m_name(std::move(name))
    , m_width(width)
    , m_height(height)
    , m_depth(depth)
    , m_source(source)
    , m_internalFormat(internalFormat)
    , m_format(format)
    , m_type(type)
{
    glGenTextures(1, &m_textureId);
    Update(data);
}

Texture::~Texture()
{
    if (m_textureId > 0 && m_source != Source::ExternalTexture)
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

auto Texture::Depth() const -> int
{
    return m_depth;
}

auto Texture::Source() const -> enum Source
{
    return m_source;
}

auto Texture::Empty() const -> bool
{
    return m_textureId == 0;
}

void Texture::Update(const void* data) const
{
    glBindTexture(m_target, m_textureId);
    switch (m_target)
    {
        case GL_TEXTURE_2D:
            glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, data);
            break;
        case GL_TEXTURE_3D:
            glTexImage3D(m_target, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_format, m_type, data);
            break;
        default:
            // Unsupported, do nothing.
            break;
    }
    glBindTexture(m_target, 0);
}

void Texture::CreateNewTexture()
{
    glGenTextures(1, &m_textureId);
    glBindTexture(m_target, m_textureId);
    switch (m_target)
    {
        case GL_TEXTURE_2D:
            glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, nullptr);
            break;
        case GL_TEXTURE_3D:
            glTexImage3D(m_target, 0, m_internalFormat, m_width, m_height, m_depth, 0, m_format, m_type, nullptr);
            break;
        default:
            // Unsupported, do nothing.
            break;
    }
    glBindTexture(m_target, 0);
}

} // namespace Renderer
} // namespace libprojectM
