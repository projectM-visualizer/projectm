#pragma once

#include <string>
#include <exception>

namespace libprojectM {
namespace UserSprites {

/**
 * @brief Exception for sprite loading errors.
 */
class SpriteException : public std::exception
{
public:
    SpriteException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~SpriteException() = default;

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


} // namespace UserSprites
} // namespace libprojectM
