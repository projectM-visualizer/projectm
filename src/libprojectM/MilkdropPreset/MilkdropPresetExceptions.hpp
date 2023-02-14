#pragma once

#include <string>
#include <exception>

/**
 * @brief Exception for preset loading errors.
 */
class MilkdropPresetLoadException : public std::exception
{
public:
    inline MilkdropPresetLoadException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~MilkdropPresetLoadException() = default;

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
    inline MilkdropCompileException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~MilkdropCompileException() = default;

    const std::string& message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};
