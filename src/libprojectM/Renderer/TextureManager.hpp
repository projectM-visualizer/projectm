#pragma once

#include "Renderer/TextureSamplerDescriptor.hpp"

#include <map>
#include <string>
#include <vector>

namespace libprojectM {
namespace Renderer {

class TextureManager
{
public:
    TextureManager() = delete;


    TextureManager(const std::vector<std::string>& textureSearchPaths);

    ~TextureManager() = default;


    void SetCurrentPresetPath(const std::string& path);


    auto GetTexture(const std::string& fullName) -> TextureSamplerDescriptor;


    auto GetRandomTexture(const std::string& randomName) -> TextureSamplerDescriptor;


    auto GetSampler(const std::string& fullName) -> std::shared_ptr<class Sampler>;


    void PurgeTextures();

private:

    struct UsageStats {
        UsageStats(uint32_t size)
            : sizeBytes(size){};

        uint32_t age{};
        uint32_t sizeBytes{};
    };


    struct ScannedFile {
        std::string filePath;
        std::string lowerCaseBaseName;
    };

    auto TryLoadingTexture(const std::string& name) -> TextureSamplerDescriptor;

    void Preload();

    auto LoadTexture(const ScannedFile& file) -> std::shared_ptr<Texture>;

    void AddTextureFile(const std::string& fileName, const std::string& baseName);

    static void ExtractTextureSettings(const std::string& qualifiedName, GLint& wrapMode, GLint& filterMode, std::string& name);

    void ScanTextures();

    std::vector<std::string> m_textureSearchPaths;
    std::string m_currentPresetDir;
    std::vector<ScannedFile> m_scannedTextureFiles;
    bool m_filesScanned{false};

    std::shared_ptr<Texture> m_placeholderTexture;
    std::map<std::string, std::shared_ptr<Texture>> m_textures;
    std::map<std::pair<GLint, GLint>, std::shared_ptr<Sampler>> m_samplers;
    std::map<std::string, UsageStats> m_textureStats;
    std::vector<std::string> m_randomTextures;
    std::vector<std::string> m_extensions{".jpg", ".jpeg", ".dds", ".png", ".tga", ".bmp", ".dib"};
};

}
}
