#pragma once

#include <string>
#include <exception>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Exception for preset loading errors.
 */
class MilkdropPresetLoadException : public std::exception
{
public:
    MilkdropPresetLoadException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~MilkdropPresetLoadException() = default;

    const char* what() const noexcept override
    {
        return m_message.c_str();
    }

    const std::string& message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};


/**
 * @brief Exception for code compilation errors.
 */
class MilkdropCompileException : public std::exception
{
public:
    MilkdropCompileException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~MilkdropCompileException() = default;

    const char* what() const noexcept override
    {
        return m_message.c_str();
    }

    const std::string& message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};

} // namespace MilkdropPreset
} // namespace libprojectM
