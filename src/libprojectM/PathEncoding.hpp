/**
 * @file PathEncoding.hpp
 * @brief Conversions between the string encodings used for file names and paths.
 *
 * Internally, projectM and the playlist library store file names and paths as UTF-8. On Windows, the
 * narrow-string API functions use the active ANSI code page instead, and the "W" API functions take
 * UTF-16 strings, so these are converted at the API boundary. When accessing the file system, UTF-8
 * paths are converted to UTF-16, so file names with any characters can be used.
 *
 * On other platforms, narrow strings are passed through unchanged, as file names are byte strings
 * there, usually in UTF-8.
 *
 * All functions are inline, as this header is used in both the core and the playlist library.
 */
#pragma once

#include <cstdio>
#include <string>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace libprojectM {
namespace PathEncoding {

#ifdef _WIN32

/**
 * @brief Converts a multibyte string in the given code page to UTF-16.
 * @param text The string to convert.
 * @param codePage The string's code page, e.g. CP_ACP or CP_UTF8.
 * @return The UTF-16 string.
 */
inline auto MultiByteToWide(const std::string& text, UINT codePage) -> std::wstring
{
    if (text.empty())
    {
        return {};
    }

    const int length = MultiByteToWideChar(codePage, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    if (length <= 0)
    {
        return {};
    }

    std::wstring wide(static_cast<size_t>(length), L'\0');
    MultiByteToWideChar(codePage, 0, text.data(), static_cast<int>(text.size()), &wide[0], length);
    return wide;
}

/**
 * @brief Converts a UTF-16 string to a multibyte string in the given code page.
 *
 * Characters that can't be represented in the code page are replaced by the code page's default
 * character, usually "?".
 * @param text The string to convert.
 * @param codePage The target code page, e.g. CP_ACP or CP_UTF8.
 * @return The converted string.
 */
inline auto WideToMultiByte(const std::wstring& text, UINT codePage) -> std::string
{
    if (text.empty())
    {
        return {};
    }

    const int length = WideCharToMultiByte(codePage, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    if (length <= 0)
    {
        return {};
    }

    std::string narrow(static_cast<size_t>(length), '\0');
    WideCharToMultiByte(codePage, 0, text.data(), static_cast<int>(text.size()), &narrow[0], length, nullptr, nullptr);
    return narrow;
}

/**
 * @brief Converts a UTF-8 string to UTF-16.
 * @param utf8 The UTF-8 string.
 * @return The UTF-16 string.
 */
inline auto Utf8ToWide(const std::string& utf8) -> std::wstring
{
    return MultiByteToWide(utf8, CP_UTF8);
}

/**
 * @brief Converts a UTF-16 string to UTF-8.
 * @param wide The UTF-16 string, or nullptr.
 * @return The UTF-8 string. Empty if wide is nullptr.
 */
inline auto WideToUtf8(const wchar_t* wide) -> std::string
{
    if (wide == nullptr)
    {
        return {};
    }
    return WideToMultiByte(wide, CP_UTF8);
}

/**
 * @brief Converts a UTF-16 string to UTF-8.
 * @param wide The UTF-16 string.
 * @return The UTF-8 string.
 */
inline auto WideToUtf8(const std::wstring& wide) -> std::string
{
    return WideToMultiByte(wide, CP_UTF8);
}

#endif

/**
 * @brief Converts a string passed to a narrow-string API function to UTF-8.
 *
 * On Windows, narrow strings are in the active ANSI code page. Elsewhere they are used as-is.
 * @param text The string from the API, or nullptr.
 * @return The UTF-8 string. Empty if text is nullptr.
 */
inline auto FromNarrowApi(const char* text) -> std::string
{
    if (text == nullptr)
    {
        return {};
    }
#ifdef _WIN32
    return WideToMultiByte(MultiByteToWide(text, CP_ACP), CP_UTF8);
#else
    return text;
#endif
}

/**
 * @brief Converts a UTF-8 string to the encoding of the narrow-string API functions.
 *
 * On Windows, this is the active ANSI code page, and characters it can't represent are replaced,
 * usually by "?". Elsewhere the string is returned as-is.
 * @param utf8 The UTF-8 string.
 * @return The string for the narrow API.
 */
inline auto ToNarrowApi(const std::string& utf8) -> std::string
{
#ifdef _WIN32
    return WideToMultiByte(MultiByteToWide(utf8, CP_UTF8), CP_ACP);
#else
    return utf8;
#endif
}

/**
 * @brief Creates a file system path from a UTF-8 string.
 * @tparam Path The path type, std::filesystem::path or boost::filesystem::path.
 * @param utf8 The UTF-8 path.
 * @return The path object.
 */
template<class Path>
auto ToPath(const std::string& utf8) -> Path
{
#ifdef _WIN32
    return Path(Utf8ToWide(utf8));
#else
    return Path(utf8);
#endif
}

/**
 * @brief Returns a file system path as a UTF-8 string.
 *
 * Unlike path::string() on Windows, this works for any file name, also with characters that the
 * active ANSI code page can't represent.
 * @tparam Path The path type, std::filesystem::path or boost::filesystem::path.
 * @param path The path object.
 * @return The UTF-8 path.
 */
template<class Path>
auto FromPath(const Path& path) -> std::string
{
#ifdef _WIN32
    return WideToUtf8(path.wstring());
#else
    return path.string();
#endif
}

/**
 * @brief Opens a file by its UTF-8 path, like fopen().
 * @param utf8Path The UTF-8 path of the file.
 * @param mode The fopen() mode, e.g. "rb".
 * @return The file, or nullptr if it couldn't be opened.
 */
inline auto OpenFile(const std::string& utf8Path, const char* mode) -> FILE*
{
#ifdef _WIN32
    FILE* file{nullptr};
    if (_wfopen_s(&file, Utf8ToWide(utf8Path).c_str(), MultiByteToWide(mode, CP_ACP).c_str()) != 0)
    {
        return nullptr;
    }
    return file;
#else
    return std::fopen(utf8Path.c_str(), mode);
#endif
}

} // namespace PathEncoding
} // namespace libprojectM
