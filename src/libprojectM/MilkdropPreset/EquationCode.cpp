#include "EquationCode.hpp"

#include <utility>

namespace libprojectM {
namespace MilkdropPreset {

EquationCode::EquationCode(std::string code)
    : m_code(std::move(code))
{
}

EquationCode::EquationCode(const char* code)
    : m_code(code != nullptr ? code : "")
{
}

auto EquationCode::Code() const -> const std::string&
{
    return m_code;
}

auto EquationCode::Empty() const -> bool
{
    return m_code.empty();
}

void EquationCode::Append(const std::string& text, int presetLine, int presetColumn)
{
    if (text.empty())
    {
        return;
    }

    m_segments.push_back({m_currentLine, m_currentLineLength + 1, presetLine, presetColumn});
    m_code.append(text);
    m_currentLineLength += static_cast<int>(text.length());
}

void EquationCode::AppendLineBreak()
{
    m_code.push_back('\n');
    m_currentLine++;
    m_currentLineLength = 0;
}

auto EquationCode::PresetLocation(int compilerLine, int compilerColumn) const -> Location
{
    // 1-based column in the code line.
    const int codeColumn = compilerLine <= 1 ? compilerColumn + 1 : compilerColumn;

    // The last part that starts at or before the location.
    const Segment* match{nullptr};
    for (const auto& segment : m_segments)
    {
        if (segment.codeLine < compilerLine ||
            (segment.codeLine == compilerLine && segment.codeColumn <= codeColumn))
        {
            match = &segment;
        }
    }

    // Without a map, the code lines are the preset lines.
    if (match == nullptr)
    {
        return {compilerLine, codeColumn};
    }

    // A line without code of its own, e.g. an empty line: count lines from the last part.
    if (match->codeLine < compilerLine)
    {
        return {match->presetLine + (compilerLine - match->codeLine), codeColumn};
    }

    return {match->presetLine, match->presetColumn + (codeColumn - match->codeColumn)};
}

auto EquationCode::FormatPresetLocation(int compilerLine, int compilerColumn) const -> std::string
{
    const auto location = PresetLocation(compilerLine, compilerColumn);
    return "(L" + std::to_string(location.line) + " C" + std::to_string(location.column) + ")";
}

} // namespace MilkdropPreset
} // namespace libprojectM
