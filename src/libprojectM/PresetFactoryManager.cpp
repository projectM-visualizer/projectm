#include "PresetFactoryManager.hpp"

#include "Utils.hpp"

#include <MilkdropPreset/Factory.hpp>

#include <Logging.hpp>

#include <cassert>
#include <iostream>
#include <sstream>

namespace libprojectM {

PresetFactoryManager::~PresetFactoryManager()
{
    ClearFactories();
}

void PresetFactoryManager::ClearFactories()
{
    m_factoryMap.clear();
    for (std::vector<PresetFactory*>::iterator pos = m_factoryList.begin();
         pos != m_factoryList.end(); ++pos)
    {
        assert(*pos);
        delete (*pos);
    }
    m_factoryList.clear();
}

void PresetFactoryManager::initialize()
{
    ClearFactories();

    auto* milkdropFactory = new MilkdropPreset::Factory();
    registerFactory(milkdropFactory->supportedExtensions(), milkdropFactory);
}

void PresetFactoryManager::registerFactory(const std::string& extensions, PresetFactory* factory)
{
    // Current behavior if an extension conflict occurs is to override the previous once
    std::stringstream ss(extensions);
    std::string extension;

    m_factoryList.push_back(factory);

    while (ss >> extension)
    {
        if (m_factoryMap.count(extension))
        {
            LOG_ERROR("[PresetFactoryManager] Warning: extension \"" + extension + "\" already has a factory. New factory handler ignored.");
        }
        else
        {
            m_factoryMap.insert(std::make_pair(extension, factory));
        }
    }
}


std::unique_ptr<Preset> PresetFactoryManager::CreatePresetFromFile(const std::string& filename)
{
    try
    {
        const std::string extension = "." + ParseExtension(filename);

        return factory(extension).LoadPresetFromFile(filename);
    }
    catch (const PresetFactoryException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw PresetFactoryException(e.what());
    }
    catch (...)
    {
        throw PresetFactoryException("[PresetFactoryManager] Uncaught preset factory exception.");
    }
}

std::unique_ptr<Preset> PresetFactoryManager::CreatePresetFromStream(const std::string& extension, std::istream& data)
{
    try
    {
        return factory(extension).LoadPresetFromStream(data);
    }
    catch (const PresetFactoryException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw PresetFactoryException(e.what());
    }
    catch (...)
    {
        throw PresetFactoryException("[PresetFactoryManager] Uncaught preset factory exception.");
    }
}

PresetFactory& PresetFactoryManager::factory(const std::string& extension)
{
    if (!extensionHandled(extension))
    {
        std::string error = "[PresetFactoryManager] No preset factory associated with extension \"";
        error += extension;
        error += "\"";
        LOG_ERROR(error);
        throw PresetFactoryException(error);
    }

    return *m_factoryMap[extension];
}

bool PresetFactoryManager::extensionHandled(const std::string& extension) const
{
    return m_factoryMap.count(extension);
}

std::vector<std::string> PresetFactoryManager::extensionsHandled() const
{
    std::vector<std::string> retval;
    for (auto const& element : m_factoryMap)
    {
        retval.push_back(element.first);
    }
    return retval;
}

//CPP17: std::filesystem::path::extension
auto PresetFactoryManager::ParseExtension(const std::string& filename) -> std::string
{
    const auto start = filename.find_last_of('.');

    if (start == std::string::npos || start >= (filename.length() - 1)) {
        return "";
    }

    return Utils::ToLower(filename.substr(start + 1, filename.length()));
}

} // namespace libprojectM
