#pragma once

#include <map>
#include <string>

namespace libprojectM {


class PresetFileParser
{
public:
    using ValueMap = std::map<std::string, std::string>;

    static constexpr size_t maxFileSize = 0x100000;


    [[nodiscard]] auto Read(const std::string& presetFile) -> bool;


    [[nodiscard]] auto Read(std::istream& presetStream) -> bool;


    [[nodiscard]] auto GetCode(const std::string& keyPrefix) const -> std::string;


    [[nodiscard]] auto GetInt(const std::string& key, int defaultValue) -> int;


    [[nodiscard]] auto GetFloat(const std::string& key, float defaultValue) -> float;


    [[nodiscard]] auto GetBool(const std::string& key, bool defaultValue) -> bool;


    [[nodiscard]] auto GetString(const std::string& key, const std::string& defaultValue) -> std::string;


    auto PresetValues() const -> const ValueMap&;

protected:

    void ParseLine(const std::string& line);

private:

    static auto ToLower(std::string str) -> std::string;

    ValueMap m_presetValues;
};

}
