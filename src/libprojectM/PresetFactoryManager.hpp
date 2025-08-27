#pragma once

#include "PresetFactory.hpp"

#include <map>
#include <utility>
#include <vector>

namespace libprojectM {


class PresetFactoryException : public std::exception
{
public:
    inline PresetFactoryException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~PresetFactoryException() = default;

    const std::string& message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};


class PresetFactoryManager
{

public:
    PresetFactoryManager() = default;

    ~PresetFactoryManager();


    void initialize();





    PresetFactory& factory(const std::string& extension);




    bool extensionHandled(const std::string& extension) const;


    std::unique_ptr<Preset> CreatePresetFromFile(const std::string& filename);


    std::unique_ptr<Preset> CreatePresetFromStream(const std::string& extension, std::istream& data);

    std::vector<std::string> extensionsHandled() const;


private:
    void registerFactory(const std::string& extension, PresetFactory* factory);

    auto ParseExtension(const std::string& filename) -> std::string;

    mutable std::map<std::string, PresetFactory*> m_factoryMap;
    mutable std::vector<PresetFactory*> m_factoryList;
    void ClearFactories();
};

}
