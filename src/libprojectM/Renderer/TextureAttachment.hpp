
#pragma once

#include "Renderer/Texture.hpp"

#include <memory>

namespace libprojectM {
namespace Renderer {


class TextureAttachment
{
public:

    enum class AttachmentType
    {
        Color,
        Depth,
        Stencil,
        DepthStencil
    };

    TextureAttachment() = delete;
    TextureAttachment(const TextureAttachment&) = delete;
    auto operator=(const TextureAttachment&) -> TextureAttachment& = delete;


    explicit TextureAttachment(AttachmentType attachmentType, int width, int height);


    explicit TextureAttachment(GLint internalFormat, GLenum format, GLenum type, int width, int height);

    TextureAttachment(TextureAttachment&& other) = default;
    auto operator=(TextureAttachment&& other) -> TextureAttachment& = default;

    ~TextureAttachment() = default;


    auto Type() const -> AttachmentType;


    auto Texture() const -> std::shared_ptr<class Texture>;


    void Texture(const std::shared_ptr<class Texture>& texture);


    void SetSize(int width, int height);

private:

    void ReplaceTexture(int width, int height);

    AttachmentType m_attachmentType{AttachmentType::Color};
    std::shared_ptr<class Texture> m_texture{std::make_shared<class Texture>()};

    GLint m_internalFormat{};
    GLenum m_format{};
    GLenum m_type{};
};

}
}
