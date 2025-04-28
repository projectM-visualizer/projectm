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
    inline SpriteException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~SpriteException() = default;

    const std::string& message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};


} // namespace UserSprites
} // namespace libprojectM
