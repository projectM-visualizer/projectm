#pragma once

#include <string>

namespace libprojectM {
namespace Playlist {

class Item
{
public:
    Item() = delete;

    explicit Item(std::string filename);


    auto Filename() const -> std::string;


    auto operator==(const std::string& other) const -> bool;

private:
    std::string m_filename;
};

}
}
