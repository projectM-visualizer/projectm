
#pragma once

#include "Renderer/TextureAttachment.hpp"

#include <map>
#include <vector>

namespace libprojectM {
namespace Renderer {


class Framebuffer
{
public:

    Framebuffer();


    explicit Framebuffer(int framebufferCount);


    ~Framebuffer();


    int Count() const;


    void Bind(int framebufferIndex);


    void BindRead(int framebufferIndex);


    void BindDraw(int framebufferIndex);


    static void Unbind();


    auto SetSize(int width, int height) -> bool;


    auto Width() const -> int;


    auto Height() const -> int;


    auto GetAttachment(int framebufferIndex, TextureAttachment::AttachmentType type, int attachmentIndex = 0) const -> std::shared_ptr<TextureAttachment>;


    void SetAttachment(int framebufferIndex, int attachmentIndex, const std::shared_ptr<TextureAttachment>& attachment);


    void CreateColorAttachment(int framebufferIndex, int attachmentIndex);


    void CreateColorAttachment(int framebufferIndex, int attachmentIndex,
                               GLint internalFormat, GLenum format, GLenum type);


    void RemoveColorAttachment(int framebufferIndex, int attachmentIndex);


    auto GetColorAttachmentTexture(int framebufferIndex, int attachmentIndex) const -> std::shared_ptr<class Texture>;


    void CreateDepthAttachment(int framebufferIndex);


    void RemoveDepthAttachment(int framebufferIndex);


    void CreateStencilAttachment(int framebufferIndex);


    void RemoveStencilAttachment(int framebufferIndex);


    void CreateDepthStencilAttachment(int framebufferIndex);


    void RemoveDepthStencilAttachment(int framebufferIndex);


    void MaskDrawBuffer(int bufferIndex, bool masked);

private:

    void UpdateDrawBuffers(int framebufferIndex);


    void RemoveAttachment(int framebufferIndex, GLenum attachmentType);

    using AttachmentsPerSlot = std::map<GLenum, std::shared_ptr<TextureAttachment>>;
    std::vector<unsigned int> m_framebufferIds{};
    std::map<int, AttachmentsPerSlot> m_attachments;

    int m_width{};
    int m_height{};

    int m_readFramebuffer{};
    int m_drawFramebuffer{};
};

}
}
