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
class FileParser
{
public:
    using ValueMap = std::map<std::string, std::string>; //!< A map with key/value pairs, each representing one line in the preset file.

    static constexpr size_t maxFileSize = 0x100000; //!< Maximum size of a preset file. Used for sanity checks.

    /**
     * @brief Reads the preset file into an internal map to prepare for parsing.
     * @return True if the file was parsed successfully, false if an error occurred or no line could be parsed.
     */
    [[nodiscard]] bool Read(const std::string& presetFile);

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
    [[nodiscard]] std::string GetCode(const std::string& keyPrefix) const;

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
    [[nodiscard]] int GetInt(const std::string& key, int defaultValue);

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
    [[nodiscard]] float GetFloat(const std::string& key, float defaultValue);

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
    [[nodiscard]] int GetBool(const std::string& key, bool defaultValue);

    /**
     * @brief Returns a reference to the internal value map.
     * @return A reference to the internal value map.
     */
    const ValueMap& PresetValues() const;

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

    /**
     * @brief Strips an end-of-line comment from the given line.
     * @param[in,out] line The code line to be stripped of a comment.
     */
    static void StripComment(std::string& line);

    /**
     * @brief Strips all multi-line comments from the given code.
     *
     * This is also an undocumented feature of Milkdrop's equation parser. We could do it in the parser,
     * but is doesn't hurt to generally remove such comments.
     *
     * @param[in,out] code The code to be stripped of all multi-line comments.
     */
    static void StripMultilineComment(std::string& code);


    /**
     * @brief Trims any leading/trailing whitespace from the given line.
     * @param[in,out] line The code line to be trimmed.
     */
    static void Trim(std::string& line);

private:
    ValueMap m_presetValues; //!< Map with preset keys and their value.
};


