#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Equation code joined into one line like Milkdrop does, with the way back to the original lines.
 *
 * Milkdrop removes the comments ("//" or two backslashes, until the end of each line) of a code block, then joins
 * its lines without any separator before compiling it (ReadCode() and StripLinefeedCharsAndComments() in state.cpp).
 * Its preset editor wrapped long lines, often in the middle of a name or a number, and many presets rely on this.
 *
 * The start of each original line in the joined code is kept, so that the position of a compilation error can be
 * given in the original code.
 */
class EquationCode
{
public:
    /**
     * @brief A position in the original code.
     */
    struct Position {
        int line{};   //!< Line in the original code block, 1-based: N in per_frame_N.
        int column{}; //!< Column in that line, 1-based, comments included.
    };

    /**
     * @brief Joins the given code block.
     * @param code The code block, one line per original line, as returned by PresetFileParser::GetCode().
     */
    explicit EquationCode(const std::string& code);

    /**
     * @brief Returns the joined code, to be compiled.
     * @return The code without comments and line breaks.
     */
    [[nodiscard]] auto Code() const -> const std::string&;

    /**
     * @brief Returns the position in the original code of a character of the joined code.
     *
     * The column returned by projectm_eval_get_error() is the 0-based offset in the compiled code, so it can be
     * passed as is. An offset at the end of the code is placed after the last character of the last line.
     *
     * @param offset The 0-based offset in Code().
     * @return The line and column in the original code.
     */
    [[nodiscard]] auto OriginalPosition(int offset) const -> Position;

private:
    std::string m_code;               //!< The joined code.
    std::vector<size_t> m_lineStarts; //!< Offset in m_code where each original line starts.
};

} // namespace MilkdropPreset
} // namespace libprojectM
