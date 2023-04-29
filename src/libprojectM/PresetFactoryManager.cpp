//
// C++ Implementation: PresetFactoryManager
//
// Description:
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "PresetFactoryManager.hpp"

#include <MilkdropPreset/MilkdropPresetFactory.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>

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

    auto* milkdropFactory = new MilkdropPresetFactory();
    registerFactory(milkdropFactory->supportedExtensions(), milkdropFactory);
}

// Current behavior if a conflict is occurs is to override the previous request

void PresetFactoryManager::registerFactory(const std::string& extensions, PresetFactory* factory)
{

    std::stringstream ss(extensions);
    std::string extension;

    m_factoryList.push_back(factory);

    while (ss >> extension)
    {
        if (m_factoryMap.count(extension))
        {
            std::cerr << "[PresetFactoryManager] Warning: extension \"" << extension << "\" already has a factory. New factory handler ignored." << std::endl;
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
    catch (const PresetFactoryException& e)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw PresetFactoryException(e.what());
    }
    catch (...)
    {
        throw PresetFactoryException("Uncaught preset factory exception");
    }
}

std::unique_ptr<Preset> PresetFactoryManager::CreatePresetFromStream(const std::string& extension, std::istream& data)
{
    try
    {
        return factory(extension).LoadPresetFromStream(data);
    }
    catch (const PresetFactoryException& e)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw PresetFactoryException(e.what());
    }
    catch (...)
    {
        throw PresetFactoryException("Uncaught preset factory exception");
    }
}

PresetFactory& PresetFactoryManager::factory(const std::string& extension)
{

    if (!extensionHandled(extension))
    {
        std::ostringstream os;
        os << "No preset factory associated with \"" << extension << "\"." << std::endl;
        throw PresetFactoryException(os.str());
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
    std::string ext = filename.substr(start + 1, filename.length());
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
    return ext;
}
