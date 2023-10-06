#include "TextureManager.hpp"

#include "IdleTextures.hpp"

#include "MilkdropPreset/MilkdropNoise.hpp"

#include "SOIL2/SOIL2.h"

#include "Texture.hpp"
#include "projectM-opengl.h"

#include <algorithm>
#include <memory>
#include <random>
#include <vector>

// Missing in macOS SDK. Query will most certainly fail, but then use the default format.
#ifndef GL_TEXTURE_IMAGE_FORMAT
#define GL_TEXTURE_IMAGE_FORMAT 0x828F
#endif


TextureManager::TextureManager(const std::vector<std::string>& textureSearchPaths)
    : m_textureSearchPaths(textureSearchPaths)
    , m_placeholderTexture(std::make_shared<Texture>("placeholder", 1, 1, false))
{
    Preload();
}

void TextureManager::SetCurrentPresetPath(const std::string& path)
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
    m_samplers.emplace(std::pair<GLint, GLint>(GL_CLAMP_TO_EDGE, GL_LINEAR), std::make_shared<Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR));
    m_samplers.emplace(std::pair<GLint, GLint>(GL_CLAMP_TO_EDGE, GL_NEAREST), std::make_shared<Sampler>(GL_CLAMP_TO_EDGE, GL_NEAREST));
    m_samplers.emplace(std::pair<GLint, GLint>(GL_REPEAT, GL_LINEAR), std::make_shared<Sampler>(GL_REPEAT, GL_LINEAR));
    m_samplers.emplace(std::pair<GLint, GLint>(GL_REPEAT, GL_NEAREST), std::make_shared<Sampler>(GL_REPEAT, GL_NEAREST));

    int width{};
    int height{};

    unsigned int tex = SOIL_load_OGL_texture_from_memory(
        M_data,
        M_bytes,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MULTIPLY_ALPHA, &width, &height);


    auto newTex = std::make_shared<Texture>("idlem", tex, GL_TEXTURE_2D, width, height, false);
    m_textures["idlem"] = newTex;

    tex = SOIL_load_OGL_texture_from_memory(
        headphones_data,
        headphones_bytes,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MULTIPLY_ALPHA, &width, &height);

    newTex = std::make_shared<Texture>("idleheadphones", tex, GL_TEXTURE_2D, width, height, false);
    m_textures["idleheadphones"] = newTex;

    auto noise = std::make_unique<MilkdropNoise>();

#if !USE_GLES
    // Query preferred internal texture format. GLES 3 only supports GL_RENDERBUFFER here, no texture targets.
    // That's why we use GL_BGRA as default, as this is the best general-use format according to Khronos.
    GLint preferredInternalFormat{GL_BGRA};
    glGetInternalformativ(GL_TEXTURE_2D, GL_RGBA8, GL_TEXTURE_IMAGE_FORMAT, sizeof(preferredInternalFormat), &preferredInternalFormat);
#else
    // GLES only supports GL_RGB and GL_RGBA, so we always use the latter.
    GLint preferredInternalFormat{GL_RGBA};
#endif

    GLuint noise_texture_lq_lite;
    glGenTextures(1, &noise_texture_lq_lite);
    glBindTexture(GL_TEXTURE_2D, noise_texture_lq_lite);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 32, 32, 0, preferredInternalFormat, GL_UNSIGNED_BYTE, noise->noise_lq_lite);
    auto textureNoise_lq_lite = std::make_shared<Texture>("noise_lq_lite", noise_texture_lq_lite, GL_TEXTURE_2D, 32, 32, false);
    m_textures["noise_lq_lite"] = textureNoise_lq_lite;

    GLuint noise_texture_lq;
    glGenTextures(1, &noise_texture_lq);
    glBindTexture(GL_TEXTURE_2D, noise_texture_lq);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, preferredInternalFormat, GL_UNSIGNED_BYTE, noise->noise_lq);
    auto textureNoise_lq = std::make_shared<Texture>("noise_lq", noise_texture_lq, GL_TEXTURE_2D, 256, 256, false);
    m_textures["noise_lq"] = textureNoise_lq;

    GLuint noise_texture_mq;
    glGenTextures(1, &noise_texture_mq);
    glBindTexture(GL_TEXTURE_2D, noise_texture_mq);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, preferredInternalFormat, GL_UNSIGNED_BYTE, noise->noise_mq);
    auto textureNoise_mq = std::make_shared<Texture>("noise_mq", noise_texture_mq, GL_TEXTURE_2D, 256, 256, false);
    m_textures["noise_mq"] = textureNoise_mq;

    GLuint noise_texture_hq;
    glGenTextures(1, &noise_texture_hq);
    glBindTexture(GL_TEXTURE_2D, noise_texture_hq);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, preferredInternalFormat, GL_UNSIGNED_BYTE, noise->noise_hq);
    auto textureNoise_hq = std::make_shared<Texture>("noise_hq", noise_texture_hq, GL_TEXTURE_2D, 256, 256, false);
    m_textures["noise_hq"] = textureNoise_hq;

    GLuint noise_texture_lq_vol;
    glGenTextures(1, &noise_texture_lq_vol);
    glBindTexture(GL_TEXTURE_3D, noise_texture_lq_vol);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, 32, 32, 32, 0, preferredInternalFormat, GL_UNSIGNED_BYTE, noise->noise_lq_vol);
    auto textureNoise_lq_vol = std::make_shared<Texture>("noisevol_lq", noise_texture_lq_vol, GL_TEXTURE_3D, 32, 32, false);
    m_textures["noisevol_lq"] = textureNoise_lq_vol;

    GLuint noise_texture_hq_vol;
    glGenTextures(1, &noise_texture_hq_vol);
    glBindTexture(GL_TEXTURE_3D, noise_texture_hq_vol);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, 32, 32, 32, 0, preferredInternalFormat, GL_UNSIGNED_BYTE, noise->noise_hq_vol);

    auto textureNoise_hq_vol = std::make_shared<Texture>("noisevol_hq", noise_texture_hq_vol, GL_TEXTURE_3D, 32, 32, false);
    m_textures["noisevol_hq"] = textureNoise_hq_vol;
}

void TextureManager::PurgeTextures()
{
    // Increment age of all textures
    for (auto& texture : m_textures)
    {
        if (texture.second->IsUserTexture())
        {
            m_textureStats.at(texture.first).age++;
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
    // Purge one texture. No need to inform presets, as the texture shouldn't be in use anymore.
    // If this really happens for some reason, it'll simply be reloaded on the next frame.
    m_textures.erase(m_textures.find(biggestName));
    m_textureStats.erase(m_textureStats.find(biggestName));

#ifdef DEBUG
    std::cerr << "Purged texture " << biggestName << std::endl;
#endif
}

TextureSamplerDescriptor TextureManager::TryLoadingTexture(const std::string& name)
{
    TextureSamplerDescriptor texDesc;
    GLint wrapMode{0};
    GLint filterMode{0};
    std::string unqualifiedName;

    ExtractTextureSettings(name, wrapMode, filterMode, unqualifiedName);

    ScanTextures();

    std::string lowerCaseFileName(name);
    std::transform(lowerCaseFileName.begin(), lowerCaseFileName.end(), lowerCaseFileName.begin(), tolower);

    for (const auto& file : m_scannedTextureFiles)
    {
        if (file.lowerCaseBaseName != lowerCaseFileName)
        {
            continue;
        }

        texDesc = LoadTexture(file.filePath, name);

        if (!texDesc.Empty())
        {
#ifdef DEBUG
            std::cerr << "Loaded texture " << unqualifiedName << std::endl;
#endif
            return texDesc;
        }
    }

#ifdef DEBUG
    std::cerr << "Failed to find texture " << unqualifiedName << std::endl;
#endif

    // Return a placeholder.
    return {m_placeholderTexture, m_samplers.at({wrapMode, filterMode}), name, unqualifiedName};
}

TextureSamplerDescriptor TextureManager::LoadTexture(const std::string& fileName, const std::string& name)
{
    GLint wrapMode;
    GLint filterMode;
    std::string unqualifiedName;

    ExtractTextureSettings(name, wrapMode, filterMode, unqualifiedName);
    auto sampler = m_samplers.at({wrapMode, filterMode});
    if (m_textures.find(name) != m_textures.end())
    {
        return {m_textures.at(name), sampler, name, unqualifiedName};
    }

    int width{};
    int height{};

    unsigned int const tex = SOIL_load_OGL_texture(
        fileName.c_str(),
        SOIL_LOAD_RGBA,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MULTIPLY_ALPHA, &width, &height);

    if (tex == 0)
    {
        return {};
    }

    uint32_t memoryBytes = width * height * 4; // RGBA, unsigned byte color channels.
    auto newTexture = std::make_shared<Texture>(unqualifiedName, tex, GL_TEXTURE_2D, width, height, true);
    m_textures[name] = newTexture;
    m_textureStats.insert({name, {memoryBytes}});

    return {newTexture, sampler, name, unqualifiedName};
}

auto TextureManager::GetRandomTexture(const std::string& randomName) -> TextureSamplerDescriptor
{
    std::string selectedFilename;

    std::random_device rndDevice;
    std::default_random_engine rndEngine(rndDevice());

    ScanTextures();

    std::string lowerCaseName(randomName);
    std::transform(lowerCaseName.begin(), lowerCaseName.end(), lowerCaseName.begin(), tolower);

    if (m_scannedTextureFiles.empty())
    {
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
        return {};
    }

    // Use selected filename to load the texture.
    auto desc = GetTexture(selectedFilename);

    // Create new descriptor with the original "rand00[_prefix]" name.
    return {desc.Texture(), desc.Sampler(), randomName, randomName};
}

void TextureManager::AddTextureFile(const std::string& fileName, const std::string& baseName)
{
    std::string lowerCaseBaseName(baseName);
    std::transform(lowerCaseBaseName.begin(), lowerCaseBaseName.end(), lowerCaseBaseName.begin(), tolower);

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

    std::string lowerQualifiedName(qualifiedName);
    std::transform(lowerQualifiedName.begin(), lowerQualifiedName.end(), lowerQualifiedName.begin(), tolower);

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
        fileScanner.scan(std::bind(&TextureManager::AddTextureFile, this, _1, _2));
        m_filesScanned = true;
    }
}
