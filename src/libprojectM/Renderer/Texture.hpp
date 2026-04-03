/**
* @file Texture.hpp
* @brief Defines a class to hold a GPU texture.
*/
#pragma once

#include "Renderer/Sampler.hpp"

#include <string>

namespace libprojectM {
namespace Renderer {

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
     * @param target The texture target, either TEXTURE_2D or TEXTURE_3D.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param depth Depth in pixels (for 3D textures).
     * @param internalFormat OpenGL internal texture format.
     * @param format OpenGL texture format.
     * @param type Storage type for each color channel.
     * @param isUserTexture true if the texture is an externally-loaded image, false if it's an internal texture.
     */
    explicit Texture(std::string name, GLenum target, int width, int height, int depth,
                     GLint internalFormat, GLenum format, GLenum type, bool isUserTexture);

    /**
     * @brief Constructor. Creates a new texture instance from an existing OpenGL texture.
     * @param name Optional name of the texture for referencing in Milkdrop shaders.
     * @param texID The OpenGL texture name (ID).
     * @param target The texture target type, e.g. GL_TEXTURE_2D.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param isUserTexture true if the texture is an externally-loaded image, false if it's an internal texture.
     * @param owned If true (default), the class takes ownership and will delete the texture when destroyed.
     *              If false, the texture is managed externally and won't be deleted.
     */
    explicit Texture(std::string name, GLuint texID, GLenum target,
                     int width, int height,
                     bool isUserTexture, bool owned = true);

    /**
     * @brief Constructor. Creates a new texture from image data with the given size and format.
     * @param name Optional name of the texture for referencing in Milkdrop shaders.
     * @param data The texture data to upload.
     * @param target The texture target, either TEXTURE_2D or TEXTURE_3D.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param depth Depth in pixels (for 3D textures).
     * @param internalFormat OpenGL internal texture format.
     * @param format OpenGL texture format.
     * @param type Storage type for each color channel.
     * @param isUserTexture true if the texture is an externally-loaded image, false if it's an internal texture.
     */
    explicit Texture(std::string name, const void* data, GLenum target, int width, int height, int depth,
                     GLint internalFormat, GLenum format, GLenum type, bool isUserTexture);

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
     * @brief Returns the depth of the texture image in pixels.
     * @return The depth of the texture image in pixels.
     */
    auto Depth() const -> int;

    /**
     * @brief Returns if the texture is user-defined, e.g. loaded from an image.
     * @return true if the texture is a user texture, false if it's an internally generated texture.
     */
    auto IsUserTexture() const -> bool;

    /**
     * @brief Returns true if the texture is empty/unallocated.
     * @return true if the texture is not yet allocated (e.g. the ID 0), false if the object contains a valid texture.
     */
    auto Empty() const -> bool;

    /**
     * @brief Uploads new image data for the texture.
     * @note Automatically binds and unbinds the texture.
     * @param data The new texture image data. Must match the size and format used to create the texture.
     */
    void Update(const void* data) const;

private:
    /**
     * @brief Creates a new, blank texture with the given size.
     */
    void CreateNewTexture();

    GLuint m_textureId{0};    //!< The OpenGL texture name/ID.
    GLenum m_target{GL_NONE}; //!< The OpenGL texture target, e.g. GL_TEXTURE_2D.

    std::string m_name;          //!< The texture name for identifying it in shaders.
    int m_width{0};              //!< Texture width in pixels.
    int m_height{0};             //!< Texture height in pixels.
    int m_depth{0};              //!< Texture depth in pixels. Only used for 3D textures.
    bool m_isUserTexture{false}; //!< true if it's a user texture, false if an internal one.
    bool m_owned{true};          //!< true if this class owns the texture and should delete it.

    GLint m_internalFormat{}; //!< OpenGL internal format, e.g. GL_RGBA8
    GLenum m_format{};        //!< OpenGL color format, e.g. GL_RGBA
    GLenum m_type{};          //!< OpenGL component storage type, e.g. GL_UNSIGNED _BYTE
};

} // namespace Renderer
} // namespace libprojectM
