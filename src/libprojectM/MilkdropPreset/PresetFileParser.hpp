#pragma once

#include <map>
#include <string>

/**
 * @brief Milkdrop preset file parser
 *
 * Reads in the file as key/value pairs, where the key is either separated from the value by an equal sign or a space.
 * Lines not matching this pattern are simply ignored, e.g. the [preset00] INI section.
 *
 * Values and code blocks can easily be accessed via the helper functions. It is also possible to access the parsed
 * map contents directly if required.
 */
class PresetFileParser
{
public:
    using ValueMap = std::map<std::string, std::string>; //!< A map with key/value pairs, each representing one line in the preset file.

    static constexpr size_t maxFileSize = 0x100000; //!< Maximum size of a preset file. Used for sanity checks.

    /**
     * @brief Reads the preset file into an internal map to prepare for parsing.
     * @return True if the file was parsed successfully, false if an error occurred or no line could be parsed.
     */
    [[nodiscard]] auto Read(const std::string& presetFile) -> bool;

    /**
     * @brief Reads the data stream into an internal map to prepare for parsing.
     * @return True if the stream was parsed successfully, false if an error occurred or no line could be parsed.
     */
    [[nodiscard]] auto Read(std::istream& presetStream) -> bool;

    /**
     * @brief Returns a block of code, ready for parsing or use in shader compilation.
     *
     * Shaders have a "`" prepended on each line. If a line starts with this character, it's stripped and a newline
     * character is added at the end of each line. Equations are returned as a single, long line.
     *
     * The function appends numbers to the prefix, starting with 1, and stops when a key is missing. This is following
     * Milkdrop's behaviour, so any gap in numbers will essentially cut off all code after the gap.
     *
     * Comments starting with // or \\\\ will be stripped until the end of each line in both equations and shader code.
     *
     * @param keyPrefix The key prefix for the code block to be returned.
     * @return The code that was parsed from the given prefix. Empty if no code was found.
     */
    [[nodiscard]] auto GetCode(const std::string& keyPrefix) const -> std::string;

    /**
     * @brief Returns the given key value as an integer.
     *
     * Returns the default value if no value can be parsed or the key doesn't exist.
     *
     * Any additional text after the number, e.g. a comment, is ignored.
     *
     * @param key The key to retrieve the value from.
     * @param defaultValue The default value to return if key is not found.
     * @return The converted value or the default value.
     */
    [[nodiscard]] auto GetInt(const std::string& key, int defaultValue) -> int;

    /**
     * @brief Returns the given key value as a floating-point value.
     *
     * Returns the default value if no value can be parsed or the key doesn't exist.
     *
     * Any additional text after the number, e.g. a comment, is ignored.
     *
     * @param key The key to retrieve the value from.
     * @param defaultValue The default value to return if key is not found.
     * @return The converted value or the default value.
     */
    [[nodiscard]] auto GetFloat(const std::string& key, float defaultValue) -> float;

    /**
     * @brief Returns the given key value as a boolean.
     *
     * Returns the default value if no value can be parsed or the key doesn't exist.
     *
     * Any additional text after the number, e.g. a comment, is ignored.
     *
     * @param key The key to retrieve the value from.
     * @param defaultValue The default value to return if key is not found.
     * @return True if the value is non-zero, false otherwise.
     */
    [[nodiscard]] auto GetBool(const std::string& key, bool defaultValue) -> bool;

    /**
     * @brief Returns the given key value as a string.
     *
     * Returns the default value if no value can be parsed or the key doesn't exist.
     *
     * @param key The key to retrieve the value from.
     * @param defaultValue The default value to return if key is not found.
     * @return the string content of the key, or the default value.
     */
    [[nodiscard]] auto GetString(const std::string& key, const std::string& defaultValue) -> std::string;

    /**
     * @brief Returns a reference to the internal value map.
     * @return A reference to the internal value map.
     */
    auto PresetValues() const -> const ValueMap&;

protected:
    /**
     * @brief Parses a single line and stores the result in the value map.
     *
     * The function doesn't really care about invalid lines with random text or comments. The first "word"
     * is added as key to the map, but will not be used afterwards.
     *
     * @param line The line to parse.
     */
    void ParseLine(const std::string& line);

private:
    ValueMap m_presetValues; //!< Map with preset keys and their value.
};
