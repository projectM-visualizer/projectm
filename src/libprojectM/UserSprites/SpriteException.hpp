#pragma once

#include <exception>
#include <string>

namespace libprojectM {
namespace UserSprites {


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


}
}
