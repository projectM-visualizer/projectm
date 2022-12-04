#include "Filter.hpp"

#include <cstring>

namespace ProjectM {
namespace Playlist {

auto Filter::List() const -> const std::vector<std::string>&
{
    return m_filters;
}

void Filter::SetList(std::vector<std::string> filterList)
{
    m_filters = std::move(filterList);
}


auto Filter::Passes(const std::string& filename) -> bool
{
    for (const auto& filterExpression : m_filters)
    {
        if (!filterExpression.empty() && ApplyExpression(filename, filterExpression))
        {
            // Default action is "remove if filename matches".
            return filterExpression.at(0) == '+';
        }
    }

    return true;
}


auto Filter::ApplyExpression(const std::string& filename, const std::string& filterExpression) -> bool
{
    // Implementation idea thanks to Robert van Engelen
    // https://www.codeproject.com/Articles/5163931/Fast-String-Matching-with-Wildcards-Globs-and-Giti

    if (filename.empty() || filterExpression.empty())
    {
        return false;
    }

    const auto* currentFilenameChar{filename.c_str()};
    const auto* currentFilterChar{filterExpression.c_str()};

    const char* previousFilenameChar{nullptr};
    const char* previousFilterChar{nullptr};

    bool inPathglob{false}; //!< True if the glob has a '**' pattern

    auto isPathSep = [](const char* character) {
        return *character == '/' || *character == '\\';
    };

    if (*currentFilterChar == '+' || *currentFilterChar == '-')
    {
        currentFilterChar++;
    }

    if (isPathSep(currentFilterChar))
    {
        while (*currentFilenameChar == '.' && isPathSep(&currentFilenameChar[1]))
        {
            currentFilenameChar += 2;
        }
        while (isPathSep(currentFilenameChar))
        {
            currentFilenameChar++;
        }
        currentFilterChar++;
    }
    else if (strchr(currentFilterChar, '/') == nullptr && strchr(currentFilterChar, '\\') == nullptr)
    {
        const auto *separatorUnix = strrchr(currentFilenameChar, '/');
        const auto *separatorwindows = strrchr(currentFilenameChar, '\\');
        if (separatorUnix != nullptr && separatorwindows != nullptr)
        {
            currentFilenameChar = std::min(separatorUnix, separatorwindows) + 1;
        }
        else if (separatorUnix != nullptr)
        {
            currentFilenameChar = separatorUnix + 1;
        }
        else if (separatorwindows != nullptr)
        {
            currentFilenameChar = separatorwindows + 1;
        }
    }

    while (*currentFilenameChar != '\0')
    {
        switch (*currentFilterChar)
        {
            case '*':
                previousFilenameChar = currentFilenameChar;
                previousFilterChar = currentFilterChar;

                inPathglob = false;
                currentFilterChar++;
                if (*currentFilterChar == '*')
                {
                    currentFilterChar++;
                    if (*currentFilterChar == '\0')
                    {
                        return true;
                    }
                    if (!isPathSep(currentFilterChar))
                    {
                        return false;
                    }

                    inPathglob = true;
                    currentFilterChar++;
                }

                continue;

            case '?':
                if (isPathSep(currentFilenameChar))
                {
                    break;
                }

                currentFilenameChar++;
                currentFilterChar++;

            default:
                if (*currentFilterChar != *currentFilenameChar &&
                    !(isPathSep(currentFilterChar) && isPathSep(currentFilenameChar)))
                {
                    break;
                }
                currentFilenameChar++;
                currentFilterChar++;
                continue;
        }

        if (previousFilterChar != nullptr && (inPathglob || !isPathSep(previousFilenameChar)))
        {
            currentFilenameChar = ++previousFilenameChar;
            currentFilterChar = previousFilterChar;
            continue;
        }

        return false;
    }

    while (*currentFilterChar == '*')
    {
        currentFilterChar++;
    }

    return *currentFilterChar == '\0';
}


} // namespace Playlist
} // namespace ProjectM
