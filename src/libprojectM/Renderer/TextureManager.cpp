#include "Renderer/TextureManager.hpp"

#include "Renderer/FileScanner.hpp"
#include "Renderer/IdleTextures.hpp"
#include "Renderer/MilkdropNoise.hpp"
#include "Renderer/Texture.hpp"

#include <Logging.hpp>
#include <Utils.hpp>

#include <stb_image.h>

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <random>
#include <vector>

// Missing in macOS SDK. Query will most certainly fail, but then use the default format.
#ifndef GL_TEXTURE_IMAGE_FORMAT
#define GL_TEXTURE_IMAGE_FORMAT 0x828F
#endif

namespace libprojectM {
namespace Renderer {

TextureManager::TextureManager(const std::vector<std::string>& textureSearchPaths)
    : m_textureSearchPaths(textureSearchPaths)
    , m_placeholderTexture(std::make_shared<Texture>("placeholder", 1, 1, Texture::Source::Internal))
{
    Preload();
}

void TextureManager::SetCurrentPresetPath(const std::string&)
{
}

TextureSamplerDescriptor TextureManager::GetTexture(const std::string& fullName)
{
    std::string unqualifiedName;
    GLint wrapMode;
    GLint filterMode;

    ExtractTextureSettings(fullName, wrapMode, filterMode, unqualifiedName);
    if (m_textures.find(unqualifiedName) == m_textures.end())
    {
        return TryLoadingTexture(fullName);
    }

    return {m_textures[unqualifiedName], m_samplers.at({wrapMode, filterMode}), fullName, unqualifiedName};
}

auto TextureManager::GetSampler(const std::string& fullName) -> std::shared_ptr<class Sampler>
{
    std::string unqualifiedName;
    GLint wrapMode;
    GLint filterMode;

    ExtractTextureSettings(fullName, wrapMode, filterMode, unqualifiedName);

    return m_samplers.at({wrapMode, filterMode});
}

void TextureManager::Preload()
{
    // Create samplers
    m_samplers.emplace(std::make_pair(GL_CLAMP_TO_EDGE, GL_LINEAR), std::make_shared<Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR));
    m_samplers.emplace(std::make_pair(GL_CLAMP_TO_EDGE, GL_NEAREST), std::make_shared<Sampler>(GL_CLAMP_TO_EDGE, GL_NEAREST));
    m_samplers.emplace(std::make_pair(GL_REPEAT, GL_LINEAR), std::make_shared<Sampler>(GL_REPEAT, GL_LINEAR));
    m_samplers.emplace(std::make_pair(GL_REPEAT, GL_NEAREST), std::make_shared<Sampler>(GL_REPEAT, GL_NEAREST));

    int width{};
    int height{};
    int channels{};

    {
        std::unique_ptr<stbi_uc, decltype(&free)> const imageData(stbi_load_from_memory(M_data, M_bytes, &width, &height, &channels, 0), free);

        if (imageData.get() != nullptr)
        {
            auto format = TextureFormatFromChannels(channels);
            m_textures["idlem"] = std::make_shared<Texture>("idlem", reinterpret_cast<const void*>(imageData.get()), GL_TEXTURE_2D, width, height, 0, format, format, GL_UNSIGNED_BYTE, Texture::Source::Internal);
        }
    }

    {
        std::unique_ptr<stbi_uc, decltype(&free)> const imageData(stbi_load_from_memory(headphones_data, headphones_bytes, &width, &height, &channels, 0), free);

        if (imageData.get() != nullptr)
        {
            auto format = TextureFormatFromChannels(channels);
            m_textures["idleheadphones"] = std::make_shared<Texture>("idleheadphones", reinterpret_cast<const void*>(imageData.get()), GL_TEXTURE_2D, width, height, 0, format, format, GL_UNSIGNED_BYTE, Texture::Source::Internal);
        }
    }

    // Noise textures
    m_textures["noise_lq_lite"] = MilkdropNoise::LowQualityLite();
    m_textures["noise_lq"] = MilkdropNoise::LowQuality();
    m_textures["noise_mq"] = MilkdropNoise::MediumQuality();
    m_textures["noise_hq"] = MilkdropNoise::HighQuality();
    m_textures["noisevol_lq"] = MilkdropNoise::LowQualityVolume();
    m_textures["noisevol_hq"] = MilkdropNoise::HighQualityVolume();
}

void TextureManager::PurgeTextures()
{
    // Increment age of all purgeable textures
    for (auto& textureName : m_purgeableTextures)
    {
        auto texture = m_textures.find(textureName);
        if (texture == m_textures.end())
        {
            continue;
        }

        if (texture->second->Source() != Texture::Source::Internal)
        {
            m_textureStats.at(textureName).age++;
        }
    }

    // Clear file cache
    m_scannedTextureFiles.clear();
    m_filesScanned = false;

    // Only purge textures with an age of 2 or higher, so we don't evict textures used by the preset being blended out
    uint32_t newest = 99999999;
    uint32_t oldest = 0;
    bool foundTextureToEvict = false;
    for (const auto& stat : m_textureStats)
    {
        if (stat.second.sizeBytes > 0 && stat.second.age > 1)
        {
            newest = std::min(newest, stat.second.age);
            oldest = std::max(oldest, stat.second.age);
            foundTextureToEvict = true;
        }
    }

    if (!foundTextureToEvict)
    {
        return;
    }

    uint32_t biggestBytes = 0;
    std::string biggestName;
    for (const auto& stat : m_textureStats)
    {
        if (stat.second.sizeBytes > 0 && stat.second.age > 1)
        {
            auto sizeMultiplicator = 1.0f + static_cast<float>(stat.second.age - newest) / static_cast<float>(oldest - newest);
            auto scaledSize = static_cast<uint32_t>(stat.second.sizeBytes * sizeMultiplicator);
            if (scaledSize > biggestBytes)
            {
                biggestBytes = scaledSize;
                biggestName = stat.first;
            }
        }
    }

    if (biggestName.empty())
    {
        return;
    }

    // Purge one texture. No need to inform presets, as the texture will stay alive until the preset
    // is unloaded.
    auto textureToEvict = m_textures.find(biggestName);
    if (textureToEvict == m_textures.end() || ! textureToEvict->second)
    {
        LOG_DEBUG("[TextureManager] Wanted to purge texture \"" + biggestName + "\", but could not find it anymore.");
        return;
    }

    enum Texture::Source source = textureToEvict->second->Source();
    m_textures.erase(textureToEvict);
    m_textureStats.erase(m_textureStats.find(biggestName));

    if (m_textureUnloadCallback && source != Texture::Source::PresetRequested)
    {
        m_textureUnloadCallback(biggestName);
    }

    LOG_DEBUG("[TextureManager] Purged texture \"" + biggestName + "\"");
}

void TextureManager::SetTextureLoadCallback(TextureLoadCallback callback)
{
    m_textureLoadCallback = std::move(callback);
}

auto TextureManager::LoadExternalTextureRaw(const std::string& unqualifiedName, const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels) -> bool
{
    if (channels != 3 && channels != 4)
    {
        LOG_ERROR("[TextureManager] Texture must have 3 or 4 channels!");
        return false;
    }

    if (width == 0 || height == 0 || width > 8192 || height > 8192)
    {
        LOG_ERROR("[TextureManager] Invalid texture size, allowed width/height is 1 to 8192 pixels each!");
        return false;
    }

    auto format = TextureFormatFromChannels(channels);
    auto newTexture = std::make_shared<Texture>(unqualifiedName,
                                                reinterpret_cast<const void*>(data),
                                                GL_TEXTURE_2D, width, height, 0,
                                                format, format, GL_UNSIGNED_BYTE, Texture::Source::ExternalImage);
    if (!newTexture->Empty())
    {
        m_textures[lowerCaseUnqualifiedName] = newTexture;
        uint32_t memoryBytes = width * height * channels;
        m_textureStats.insert({lowerCaseUnqualifiedName, {memoryBytes}});
        LOG_DEBUG("[TextureManager] Loaded texture \"" + unqualifiedName + "\" from callback (pixel data)");
        return {newTexture, m_samplers.at({wrapMode, filterMode}), name, unqualifiedName};
    }

    LOG_WARN("[TextureManager] Failed to create OpenGL texture from callback pixel data for \"" + unqualifiedName + "\"; falling back to filesystem");
}

auto TextureManager::LoadExternalTextureID(const std::string& unqualifiedName, GLuint textureId, uint32_t width, uint32_t height, uint32_t channels) -> bool
{
    if (channels == 0 || channels > 4)
    {
        LOG_ERROR("[TextureManager] Texture must have 1 to 4 channels!");
        return false;
    }

    if (width == 0 || height == 0 || width > 8192 || height > 8192)
    {
        LOG_ERROR("[TextureManager] Invalid texture size, allowed width/height is 1 to 8192 pixels each!");
        return false;
    }

    std::string lowerCaseUnqualifiedName = Utils::ToLower(unqualifiedName);

    // App-provided textures are not owned by projectM
    auto newTexture = std::make_shared<Texture>(unqualifiedName, textureId,
                                                GL_TEXTURE_2D, width, height, Texture::Source::ExternalTexture);
    m_textures[lowerCaseUnqualifiedName] = newTexture;
    uint32_t memoryBytes = width * height * channels;
    m_textureStats.insert({lowerCaseUnqualifiedName, {memoryBytes}});
    LOG_DEBUG("[TextureManager] Loaded external texture \"" + unqualifiedName + "\" from texture ID)");

    return true;
}

auto TextureManager::LoadExternalTextureFile(const std::string& unqualifiedName, const uint8_t* data) -> bool
{

}

auto TextureManager::UnloadExternalTexture(const std::string& unqualifiedName) -> bool
{
}

auto TextureManager::TryLoadingTexture(const std::string& name) -> TextureSamplerDescriptor
{
    GLint wrapMode{0};
    GLint filterMode{0};
    std::string unqualifiedName;

    ExtractTextureSettings(name, wrapMode, filterMode, unqualifiedName);

    std::string lowerCaseUnqualifiedName = Utils::ToLower(unqualifiedName);

    // Try callback first if registered
    if (m_textureLoadCallback)
    {
        TextureLoadData loadData;
        m_textureLoadCallback(lowerCaseUnqualifiedName, loadData);

        // Check if callback provided an existing OpenGL texture ID
        if (loadData.textureId != 0 && loadData.width > 0 && loadData.height > 0)
        {
            // App-provided textures are not owned by projectM
            auto newTexture = std::make_shared<Texture>(unqualifiedName, loadData.textureId,
                                                        GL_TEXTURE_2D, loadData.width, loadData.height, Texture::Source::ExternalTexture);
            m_textures[lowerCaseUnqualifiedName] = newTexture;
            uint32_t memoryBytes = loadData.width * loadData.height * (loadData.channels > 0 ? loadData.channels : 4);
            m_textureStats.insert({lowerCaseUnqualifiedName, {memoryBytes}});
            LOG_DEBUG("[TextureManager] Loaded texture \"" + unqualifiedName + "\" from callback (texture ID)");
            return {newTexture, m_samplers.at({wrapMode, filterMode}), name, unqualifiedName};
        }

        if (loadData.textureId != 0)
        {
            LOG_WARN("[TextureManager] Callback provided texture ID for \"" + unqualifiedName + "\" but width/height are invalid; falling back to filesystem");
        }

        // Check if callback provided raw pixel data
        if (loadData.data != nullptr && loadData.width > 0 && loadData.height > 0)
        {
            int width = static_cast<int>(loadData.width);
            int height = static_cast<int>(loadData.height);
            int channels = static_cast<int>(loadData.channels > 0 ? loadData.channels : 4);

            auto format = TextureFormatFromChannels(channels);
            auto newTexture = std::make_shared<Texture>(unqualifiedName,
                                                        reinterpret_cast<const void*>(loadData.data),
                                                        GL_TEXTURE_2D, width, height, 0,
                                                        format, format, GL_UNSIGNED_BYTE, Texture::Source::ExternalImage);
            if (!newTexture->Empty())
            {
                m_textures[lowerCaseUnqualifiedName] = newTexture;
                uint32_t memoryBytes = width * height * channels;
                m_textureStats.insert({lowerCaseUnqualifiedName, {memoryBytes}});
                LOG_DEBUG("[TextureManager] Loaded texture \"" + unqualifiedName + "\" from callback (pixel data)");
                return {newTexture, m_samplers.at({wrapMode, filterMode}), name, unqualifiedName};
            }

            LOG_WARN("[TextureManager] Failed to create OpenGL texture from callback pixel data for \"" + unqualifiedName + "\"; falling back to filesystem");
        }
    }

    // Fall back to filesystem loading
    ScanTextures();

    for (const auto& file : m_scannedTextureFiles)
    {
        if (file.lowerCaseBaseName != lowerCaseUnqualifiedName)
        {
            continue;
        }

        auto texture = LoadTexture(file);

        if (texture)
        {
            LOG_DEBUG("[TextureManager] Loaded texture \"" + unqualifiedName + "\" from file: " + file.filePath);
            return {texture, m_samplers.at({wrapMode, filterMode}), name, unqualifiedName};
        }
    }

    LOG_WARN("[TextureManager] Failed to find requested texture \"" + unqualifiedName + "\"");

    // Return a placeholder.
    return {m_placeholderTexture, m_samplers.at({wrapMode, filterMode}), name, unqualifiedName};
}

auto TextureManager::LoadTexture(const ScannedFile& file) -> std::shared_ptr<Texture>
{
    if (m_textures.find(file.lowerCaseBaseName) != m_textures.end())
    {
        return m_textures.at(file.lowerCaseBaseName);
    }

    int width{};
    int height{};

    std::unique_ptr<stbi_uc, decltype(&free)> imageData(stbi_load(file.filePath.c_str(), &width, &height, nullptr, 4), free);

    if (imageData.get() == nullptr)
    {
        LOG_DEBUG("[TextureManager] Failed to decode image data.");
        return {};
    }

    auto format = TextureFormatFromChannels(4);
    auto newTexture = std::make_shared<Texture>(file.lowerCaseBaseName, reinterpret_cast<const void*>(imageData.get()), GL_TEXTURE_2D, width, height, 0, format, format, GL_UNSIGNED_BYTE, Texture::Source::PresetRequested);

    imageData.reset();

    uint32_t const memoryBytes = width * height * 4; // RGBA, unsigned byte color channels.
    m_textures[file.lowerCaseBaseName] = newTexture;
    m_textureStats.insert({file.lowerCaseBaseName, {memoryBytes}});

    return newTexture;
}

auto TextureManager::GetRandomTexture(const std::string& randomName) -> TextureSamplerDescriptor
{
    std::string selectedFilename;

    std::random_device rndDevice;
    std::default_random_engine rndEngine(rndDevice());

    ScanTextures();

    std::string lowerCaseName = Utils::ToLower(randomName);

    if (m_scannedTextureFiles.empty())
    {
        LOG_DEBUG("[TextureManager] Texture file list is empty.");
        return {};
    }

    std::string prefix;
    if (lowerCaseName.length() > 7 && lowerCaseName.at(6) == '_')
    {
        prefix = lowerCaseName.substr(7);
    }

    if (prefix.empty())
    {
        // Just pick a random index.
        std::uniform_int_distribution<size_t> distribution(0, m_scannedTextureFiles.size() - 1);
        selectedFilename = m_scannedTextureFiles.at(distribution(rndEngine)).lowerCaseBaseName;
    }
    else
    {

        std::vector<ScannedFile> filteredFiles;
        auto prefixLength = prefix.length();
        std::copy_if(m_scannedTextureFiles.begin(), m_scannedTextureFiles.end(),
                     std::back_inserter(filteredFiles),
                     [&prefix, prefixLength](const ScannedFile& file) {
                         return file.lowerCaseBaseName.substr(0, prefixLength) == prefix;
                     });

        if (!filteredFiles.empty())
        {
            std::uniform_int_distribution<size_t> distribution(0, filteredFiles.size() - 1);
            selectedFilename = filteredFiles.at(distribution(rndEngine)).lowerCaseBaseName;
        }
    }

    // If a prefix was set and no file matched, filename can be empty.
    if (selectedFilename.empty())
    {
        LOG_DEBUG("[TextureManager] No random texture found. Prefix: \"" + prefix + "\".");
        return {};
    }

    // Use selected filename to load the texture.
    auto desc = GetTexture(selectedFilename);

    // Create new descriptor with the original "rand00[_prefix]" name.
    return {desc.Texture(), desc.Sampler(), randomName, randomName};
}

void TextureManager::AddTextureFile(const std::string& fileName, const std::string& baseName)
{
    std::string lowerCaseBaseName = Utils::ToLower(baseName);

    ScannedFile file;
    file.filePath = fileName;
    file.lowerCaseBaseName = lowerCaseBaseName;

    m_scannedTextureFiles.push_back(std::move(file));
}

void TextureManager::ExtractTextureSettings(const std::string& qualifiedName, GLint& wrapMode, GLint& filterMode, std::string& name)
{
    if (qualifiedName.length() <= 3 || qualifiedName.at(2) != '_')
    {
        name = qualifiedName;
        filterMode = GL_LINEAR;
        wrapMode = GL_REPEAT;
        return;
    }

    std::string lowerQualifiedName = Utils::ToLower(qualifiedName);

    // Default mode for user textures is "fw" (bilinear filtering + wrap).
    wrapMode = GL_REPEAT;
    filterMode = GL_LINEAR;

    if (lowerQualifiedName.substr(0, 3) == "fc_" || lowerQualifiedName.substr(0, 3) == "cf_")
    {
        name = qualifiedName.substr(3);
        filterMode = GL_LINEAR;
        wrapMode = GL_CLAMP_TO_EDGE;
    }
    else if (lowerQualifiedName.substr(0, 3) == "fw_" || lowerQualifiedName.substr(0, 3) == "wf_")
    {
        name = qualifiedName.substr(3);
        filterMode = GL_LINEAR;
        wrapMode = GL_REPEAT;
    }
    else if (lowerQualifiedName.substr(0, 3) == "pc_" || lowerQualifiedName.substr(0, 3) == "cp_")
    {
        name = qualifiedName.substr(3);
        filterMode = GL_NEAREST;
        wrapMode = GL_CLAMP_TO_EDGE;
    }
    else if (lowerQualifiedName.substr(0, 3) == "pw_" || lowerQualifiedName.substr(0, 3) == "wp_")
    {
        name = qualifiedName.substr(3);
        filterMode = GL_NEAREST;
        wrapMode = GL_REPEAT;
    }
    else
    {
        name = qualifiedName.substr(3); // Milkdrop also removes the XY_ prefix in the case nothing matches.
        filterMode = GL_LINEAR;
        wrapMode = GL_REPEAT;
    }
}

void TextureManager::ScanTextures()
{
    if (!m_filesScanned)
    {
        FileScanner fileScanner = FileScanner(m_textureSearchPaths, m_extensions);

        using namespace std::placeholders;
        fileScanner.Scan(std::bind(&TextureManager::AddTextureFile, this, _1, _2));
        m_filesScanned = true;
    }
}

uint32_t TextureManager::TextureFormatFromChannels(int channels)
{
    switch (channels)
    {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            return 0;
    }
}

} // namespace Renderer
} // namespace libprojectM
