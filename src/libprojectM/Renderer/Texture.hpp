
#pragma once

#include "Renderer/Sampler.hpp"

#include <string>

namespace libprojectM {
namespace Renderer {


class Texture
{
public:
    Texture(const Texture&) = delete;
    auto operator=(const Texture&) -> Texture& = delete;


    Texture() = default;


    explicit Texture(std::string name, int width, int height, bool isUserTexture);


    explicit Texture(std::string name, int width, int height,
                     GLint internalFormat, GLenum format, GLenum type, bool isUserTexture);


    explicit Texture(std::string name, GLuint texID, GLenum target,
                     int width, int height,
                     bool isUserTexture);

    Texture(Texture&& other) = default;
    auto operator=(Texture&& other) -> Texture& = default;

    ~Texture();


    void Bind(GLint slot, const Sampler::Ptr& sampler = nullptr) const;


    void Unbind(GLint slot) const;


    auto TextureID() const -> GLuint;


    auto Name() const -> const std::string&;


    auto Type() const -> GLenum;


    auto Width() const -> int;


    auto Height() const -> int;


    auto IsUserTexture() const -> bool;


    auto Empty() const -> bool;

private:

    void CreateNewTexture();

    GLuint m_textureId{0};
    GLenum m_target{GL_NONE};

    std::string m_name;
    int m_width{0};
    int m_height{0};
    bool m_isUserTexture{false};

    GLint m_internalFormat{};
    GLenum m_format{};
    GLenum m_type{};
};

}
}
