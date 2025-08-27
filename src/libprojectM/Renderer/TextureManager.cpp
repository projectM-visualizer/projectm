#include "TextureManager.hpp"

#include "FileScanner.hpp"
#include "IdleTextures.hpp"
#include "MilkdropNoise.hpp"
#include "Texture.hpp"
#include "Utils.hpp"

#include <SOIL2/SOIL2.h>

#include <algorithm>
#include <memory>
#include <random>
#include <vector>

#ifdef DEBUG
#include <iostream>
#endif


#ifndef GL_TEXTURE_IMAGE_FORMAT
#define GL_TEXTURE_IMAGE_FORMAT 0x828F
#endif

namespace libprojectM {
namespace Renderer {

TextureManager::TextureManager(const std::vector<std::string>& textureSearchPaths)
    : m_textureSearchPaths(textureSearchPaths)
    , m_placeholderTexture(std::make_shared<Texture>("placeholder", 1, 1, false))
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

    m_textures["idlem"] = std::make_shared<Texture>("idlem", tex, GL_TEXTURE_2D, width, height, false);
    ;

    tex = SOIL_load_OGL_texture_from_memory(
        headphones_data,
        headphones_bytes,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MULTIPLY_ALPHA, &width, &height);

    m_textures["idleheadphones"] = std::make_shared<Texture>("idleheadphones", tex, GL_TEXTURE_2D, width, height, false);
    ;


    m_textures["noise_lq_lite"] = MilkdropNoise::LowQualityLite();
    m_textures["noise_lq"] = MilkdropNoise::LowQuality();
    m_textures["noise_mq"] = MilkdropNoise::MediumQuality();
    m_textures["noise_hq"] = MilkdropNoise::HighQuality();
    m_textures["noisevol_lq"] = MilkdropNoise::LowQualityVolume();
    m_textures["noisevol_hq"] = MilkdropNoise::HighQualityVolume();
}

void TextureManager::PurgeTextures()
{

    for (auto& texture : m_textures)
    {
        if (texture.second->IsUserTexture())
        {
            m_textureStats.at(texture.first).age++;
        }
    }


    m_scannedTextureFiles.clear();
    m_filesScanned = false;


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


    m_textures.erase(m_textures.find(biggestName));
    m_textureStats.erase(m_textureStats.find(biggestName));

#ifdef DEBUG
    std::cerr << "Purged texture " << biggestName << std::endl;
#endif
}

auto TextureManager::TryLoadingTexture(const std::string& name) -> TextureSamplerDescriptor
{
    GLint wrapMode{0};
    GLint filterMode{0};
    std::string unqualifiedName;

    ExtractTextureSettings(name, wrapMode, filterMode, unqualifiedName);

    ScanTextures();

    std::string lowerCaseUnqualifiedName = Utils::ToLower(unqualifiedName);
    for (const auto& file : m_scannedTextureFiles)
    {
        if (file.lowerCaseBaseName != lowerCaseUnqualifiedName)
        {
            continue;
        }

        auto texture = LoadTexture(file);

        if (texture)
        {
#ifdef DEBUG
            std::cerr << "Loaded texture " << unqualifiedName << std::endl;
#endif
            return {texture, m_samplers.at({wrapMode, filterMode}), name, unqualifiedName};
            ;
        }
    }

#ifdef DEBUG
    std::cerr << "Failed to find texture " << unqualifiedName << std::endl;
#endif


    return {m_placeholderTexture, m_samplers.at({wrapMode, filterMode}), name, unqualifiedName};
}

auto TextureManager::LoadTexture(const ScannedFile& file) -> std::shared_ptr<Texture>
{
    std::string unqualifiedName;

    if (m_textures.find(file.lowerCaseBaseName) != m_textures.end())
    {
        return m_textures.at(file.lowerCaseBaseName);
    }

    int width{};
    int height{};

    unsigned int const tex = SOIL_load_OGL_texture(
        file.filePath.c_str(),
        SOIL_LOAD_RGBA,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MULTIPLY_ALPHA, &width, &height);

    if (tex == 0)
    {
        return {};
    }

    uint32_t memoryBytes = width * height * 4;
    auto newTexture = std::make_shared<Texture>(unqualifiedName, tex, GL_TEXTURE_2D, width, height, true);
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
        return {};
    }

    std::string prefix;
    if (lowerCaseName.length() > 7 && lowerCaseName.at(6) == '_')
    {
        prefix = lowerCaseName.substr(7);
    }

    if (prefix.empty())
    {

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


    if (selectedFilename.empty())
    {
        return {};
    }


    auto desc = GetTexture(selectedFilename);


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
        name = qualifiedName.substr(3);
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

}
}
