#pragma once

#include "PresetFactory.hpp"

#include <map>
#include <utility>
#include <vector>

/// A simple exception class to strongly type all preset factory related issues
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

/// A manager of preset factories
class PresetFactoryManager
{

public:
    PresetFactoryManager() = default;

    ~PresetFactoryManager();

    /**
     * @brief Initializes the manager.
     */
    void initialize();

    /// Requests a factory given a preset extension type
    /// \param extension a string denoting the preset suffix type
    /// \throws PresetFactoryException if the extension is unhandled
    /// \returns a valid preset factory associated with the extension
    PresetFactory& factory(const std::string& extension);

    /// Tests if an extension has been registered with a factory
    /// \param extension the file name extension to verify
    /// \returns true if a factory exists, false otherwise
    bool extensionHandled(const std::string& extension) const;

    /**
     * @brief Loads a preset by a given filename or URL.
     *
     * Supported URLs are "idle://" (loads the idle preset) and "file://". Other URL schemes will
     * throw an exception.
     *
     * @param filename The filename/URL to load.
     * @throws PresetFactoryException If any error occurs during preset loading. Exception message
     *                                contains additional details.
     * @return A valid pointer to the loaded preset.
     */
    std::unique_ptr<Preset> CreatePresetFromFile(const std::string& filename);

    /**
     * @brief Loads a preset from a stream.
     * @param extension The "original" extension. Used to determine preset data format.
     * @param data A stream with preset data to load.
     * @throws PresetFactoryException If any error occurs during preset loading. Exception message
     *                                contains additional details.
     * @return A valid pointer to the loaded preset.
     */
    std::unique_ptr<Preset> CreatePresetFromStream(const std::string& extension, std::istream& data);

    std::vector<std::string> extensionsHandled() const;


private:
    void registerFactory(const std::string& extension, PresetFactory* factory);

    auto ParseExtension(const std::string& filename) -> std::string;

    mutable std::map<std::string, PresetFactory*> m_factoryMap;
    mutable std::vector<PresetFactory*> m_factoryList;
    void ClearFactories();
};
