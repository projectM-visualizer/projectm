/**
 * @file EquationCode.hpp
 * @brief Expression code of a preset, with the location of each part in the preset file.
 */
#pragma once

#include <string>
#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Expression code (equations) of a preset, ready for compilation.
 *
 * Besides the code itself, this class knows from which preset line and column each part of the
 * code comes, so compiler errors can be reported at the location the user sees in the preset,
 * even if lines were joined or comments removed.
 */
class EquationCode
{
public:
    /**
     * @brief A location in the preset, with 1-based line and column numbers.
     *
     * The line is the number of the code line in its block (e.g. 3 for "per_frame_3="), the
     * column counts characters after the "=" of that line.
     */
    struct Location {
        int line{};   //!< 1-based line number.
        int column{}; //!< 1-based column number.
    };

    EquationCode() = default;

    /**
     * @brief Creates an object with the given code, whose line and column numbers are the preset's.
     * @param code The expression code.
     */
    EquationCode(std::string code); // NOLINT(google-explicit-constructor): Code without a location map is just a string.

    EquationCode(const char* code); // NOLINT(google-explicit-constructor)

    /**
     * @brief Returns the code.
     * @return The expression code, as passed to the compiler.
     */
    auto Code() const -> const std::string&;

    /**
     * @brief Returns whether the code is empty.
     * @return true if there is no code.
     */
    auto Empty() const -> bool;

    /**
     * @brief Appends a part of a preset line to the code.
     * @param text The text to append.
     * @param presetLine The 1-based number of the preset line the text comes from.
     * @param presetColumn The 1-based column in the preset line at which the text starts.
     */
    void Append(const std::string& text, int presetLine, int presetColumn);

    /**
     * @brief Appends a line break to the code.
     */
    void AppendLineBreak();

    /**
     * @brief Translates the error location reported by projectm_eval_get_error() into the preset location.
     *
     * The expression compiler counts columns from 0 in the first line, and from 1 in all other
     * lines, as it counts the line break itself.
     *
     * @param compilerLine The line returned by the compiler.
     * @param compilerColumn The column returned by the compiler.
     * @return The location in the preset.
     */
    auto PresetLocation(int compilerLine, int compilerColumn) const -> Location;

    /**
     * @brief Formats the preset location of a compiler error for log messages, e.g. "(L3 C14)".
     * @param compilerLine The line returned by the compiler.
     * @param compilerColumn The column returned by the compiler.
     * @return The formatted location.
     */
    auto FormatPresetLocation(int compilerLine, int compilerColumn) const -> std::string;

private:
    /**
     * @brief A part of the code that comes from one preset line.
     */
    struct Segment {
        int codeLine{};     //!< 1-based line in the code.
        int codeColumn{};   //!< 1-based column in the code line where the part starts.
        int presetLine{};   //!< 1-based preset line the part comes from.
        int presetColumn{}; //!< 1-based preset column where the part starts.
    };

    std::string m_code;               //!< The code.
    std::vector<Segment> m_segments;  //!< Where the parts of the code come from. Empty if the code isn't from a preset file.
    int m_currentLine{1};             //!< Current 1-based code line while appending.
    int m_currentLineLength{};        //!< Characters in the current code line while appending.
};

} // namespace MilkdropPreset
} // namespace libprojectM
