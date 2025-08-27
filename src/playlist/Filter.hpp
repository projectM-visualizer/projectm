#pragma once

#include <string>
#include <vector>

namespace libprojectM {
namespace Playlist {


class Filter
{
public:

    auto List() const -> const std::vector<std::string>&;


    void SetList(std::vector<std::string> filterList);


    auto Passes(const std::string& filename) -> bool;

private:

    static auto ApplyExpression(const std::string& character, const std::string& filterExpression) -> bool;

    std::vector<std::string> m_filters;
};

}
}
