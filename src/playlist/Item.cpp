#include "Item.hpp"

namespace ProjectM {
namespace Playlist {

Item::Item(std::string filename)
    : m_filename(std::move(filename))
{
}


auto Item::Filename() const -> std::string
{
    return m_filename;
}


auto Item::operator==(const std::string& other) const -> bool
{
    return m_filename == other;
}

} // namespace Playlist
} // namespace ProjectM
