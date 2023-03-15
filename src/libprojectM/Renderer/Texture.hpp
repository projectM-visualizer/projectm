/**
* @file Texture.hpp
* @brief Defines a class to hold a GPU texture.
*/
#pragma once

#include "Renderer/Sampler.hpp"

#include <memory>
#include <string>
#include <vector>

/**
 * @brief Stores a texture in the GPU and manages a few properties and the samplers.
 *
 * This object will take ownership of a single texture and manage the samplers for use in shaders.
 */
class Texture
{
public:
    Texture(const Texture&) = delete;
    auto operator=(const Texture&) -> Texture& = delete;

    /**
     * @brief Constructor. Creates an empty texture with zero size.
     */
    Texture() = default;

    /**
     * @brief Constructor. Allocates a new, empty texture with the given size.
     * @param name Optional name of the texture for referencing in Milkdrop shaders.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param isUserTexture true if the texture is an externally-loaded image, false if it's an internal texture.
     */
    explicit Texture(std::string name, int width, int height, bool isUserTexture);

    /**
     * @brief Constructor. Allocates a new, empty texture with the given size and format.
     * @param name Optional name of the texture for referencing in Milkdrop shaders.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param isUserTexture true if the texture is an externally-loaded image, false if it's an internal texture.
     */
    explicit Texture(std::string name, int width, int height,
                     GLint internalFormat, GLenum format, GLenum type, bool isUserTexture);

    /**
     * @brief Constructor. Creates a new texture instance from an existing OpenGL texture.
     * The class will take ownership of the texture, e.g. freeing it when destroyed!
     * @param name Optional name of the texture for referencing in Milkdrop shaders.
     * @param texID The OpenGL texture name (ID).
     * @param target The texture target type, e.g. GL_TEXTURE_2D.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param isUserTexture true if the texture is an externally-loaded image, false if it's an internal texture.
     */
    explicit Texture(std::string name, GLuint texID, GLenum target,
                     int width, int height,
                     bool isUserTexture);

    Texture(Texture&& other) = default;
    auto operator=(Texture&& other) -> Texture& = default;

    ~Texture();

    /**
     * @brief Binds the texture to the given texture unit.
     * Also resets the last used counter to zero.
     * @param slot The texture unit to bind the texture to.
     * @param sampler An optional sampler to bind in the same slot.
     */
    void Bind(GLint slot, const Sampler::Ptr& sampler = nullptr) const;

    /**
     * Unbinds the texture to the given texture unit.
     * @param slot The texture unit to unbind the texture from.
     */
    void Unbind(GLint slot) const;

    /**
     * @brief Returns the OpenGL texture name/ID.
     * @return The OpenGL texture name/ID.
     */
    auto TextureID() const -> GLuint;

    /**
     * @brief Returns the texture name, e.g. base filename.
     * @return The texture name, for referencing the texture in shader code etc.
     */
    auto Name() const -> const std::string&;

    /**
     * @brief Returns the OpenGL texture type.
     * @return The texture type, e.g. GL_TEXTURE_2D.
     */
    auto Type() const -> GLenum;

    /**
     * @brief Returns the width of the texture image in pixels.
     * @return The width of the texture image in pixels.
     */
    auto Width() const -> int;

    /**
     * @brief Returns the height of the texture image in pixels.
     * @return The height of the texture image in pixels.
     */
    auto Height() const -> int;

    /**
     * @brief Returns if the texture is user-defined, e.g. loaded from an image.
     * @return true if the texture is a user texture, false if it's an internally generated texture.
     */
    auto IsUserTexture() const -> bool;

private:
    void CreateEmptyTexture();

    GLuint m_textureId{0};    //!< The OpenGL texture name/ID.
    GLenum m_target{GL_NONE}; //!< The OpenGL texture target, e.g. GL_TEXTURE_2D.

    std::string m_name;          //!< The texture name for identifying it in shaders.
    int m_width{0};              //!< Texture width in pixels.
    int m_height{0};             //!< Texture height in pixels.
    bool m_isUserTexture{false}; //!< true if it's a user texture, false if an internal one.

    GLint m_internalFormat{}; //!< OpenGL internal format, e.g. GL_RGBA8
    GLenum m_format{};        //!< OpenGL color format, e.g. GL_RGBA
    GLenum m_type{};          //!< OpenGL component storage type, e.g. GL_UNSIGNED _BYTE
};
