#pragma once

#include "Renderer/FileScanner.hpp"
#include "Renderer/TextureSamplerDescriptor.hpp"

#include "projectM-opengl.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>


class TextureManager
{
public:
    TextureManager() = delete;

    /**
     * Constructor.
     * @param textureSearchPaths List of paths to search for textures. These paths are searched in the given order.
     */
    TextureManager(const std::vector<std::string>& textureSearchPaths);

    ~TextureManager() = default;

    /**
     * @brief Requests a texture and sampler with the given name.
     * Resets the texture age to zero.
     * @param fullName
     * @return
     */
    auto GetTexture(const std::string& fullName) -> TextureSamplerDescriptor;

    /**
     * @brief Returns a sampler for the given name.
     * Does not load any texture, only analyzes the prefix.
     * @param fullName The name of the sampler as used in the preset.
     * @return A sampler with the prefixed mode, or the default settings.
     */
    auto GetSampler(const std::string& fullName) -> std::shared_ptr<class Sampler>;

    /**
     * @brief Purges unused textures and increments the age counter of all stored textures.
     * Also resets the scanned texture list. Must be called exactly once per preset load.
     */
    void PurgeTextures();

private:
    /**
     * Texture usage statistics. Used to determine when to purge a texture.
     */
    struct UsageStats {
        UsageStats(uint32_t size)
            : sizeBytes(size){};

        uint32_t age{};       //!< Age of the texture. Represents the number of presets loaded since it was last retrieved.
        uint32_t sizeBytes{}; //!< The texture in-memory size in bytes.
    };

    /**
     * A scanned texture file on the disk.
     */
    struct ScannedFile {
        std::string filePath;          //!< Full path to the texture file
        std::string lowerCaseBaseName; //!< Texture base file name, lower case.
    };

    auto TryLoadingTexture(const std::string& name) -> TextureSamplerDescriptor;

    auto GetRandomTexture(const std::string& randomName) -> TextureSamplerDescriptor;

    void Preload();

    TextureSamplerDescriptor LoadTexture(const std::string& fileName, const std::string& name);

    void AddTextureFile(const std::string& fileName, const std::string& baseName);

    static void ExtractTextureSettings(const std::string& qualifiedName, GLint& wrapMode, GLint& filterMode, std::string& name);

    std::vector<std::string> m_textureSearchPaths;  //!< Search paths to scan for textures.
    std::vector<ScannedFile> m_scannedTextureFiles; //!< The cached list with scanned texture files.
    bool m_filesScanned{false};                     //!< true if files were scanned since last preset load.

    std::shared_ptr<Texture> m_placeholderTexture;                          //!< Texture used if a requested file couldn't be found. A black 1x1 texture.
    std::map<std::string, std::shared_ptr<Texture>> m_textures;             //!< All loaded textures, including generated ones.
    std::map<std::pair<GLint, GLint>, std::shared_ptr<Sampler>> m_samplers; //!< The four sampler objects for each combination of wrap and filter modes.
    std::map<std::string, UsageStats> m_textureStats;                       //!< Map with texture stats for user-loaded files.
    std::vector<std::string> m_randomTextures;
    std::vector<std::string> m_extensions{".jpg", ".jpeg", ".dds", ".png", ".tga", ".bmp", ".dib"};
};
