#include "Playlist.hpp"

#include <algorithm>

// Fall back to boost if compiler doesn't support C++17
#include FS_INCLUDE
using namespace FS_NAMESPACE::filesystem;

namespace ProjectM {
namespace Playlist {

uint32_t Playlist::Size() const
{
    return m_items.size();
}


void Playlist::Clear()
{
    m_items.clear();
}


const std::vector<Item>& Playlist::Items() const
{
    return m_items;
}


bool Playlist::AddItem(const std::string& filename, uint32_t index, bool allowDuplicates)
{
    if (filename.empty())
    {
        return false;
    }

    if (!allowDuplicates)
    {
        if (std::find(m_items.begin(), m_items.end(), filename) != m_items.end())
        {
            return false;
        }
    }

    if (index >= m_items.size())
    {
        m_items.emplace_back(filename);
    }
    else
    {
        m_items.emplace(m_items.cbegin() + index, filename);
    }

    return true;
}


auto Playlist::AddPath(const std::string& path, uint32_t index, bool recursive, bool allowDuplicates) -> uint32_t
{
    uint32_t presetsAdded{0};

    if (recursive)
    {
        for (const auto& entry : recursive_directory_iterator(path))
        {
            if (is_regular_file(entry) && entry.path().extension() == ".milk")
            {
                uint32_t currentIndex{InsertAtEnd};
                if (index < InsertAtEnd)
                {
                    currentIndex = index + presetsAdded;
                }
                if (AddItem(entry.path().string(), currentIndex, allowDuplicates))
                {
                    presetsAdded++;
                }
            }
        }
    }
    else
    {
        for (const auto& entry : directory_iterator(path))
        {
            if (is_regular_file(entry) && entry.path().extension() == ".milk")
            {
                uint32_t currentIndex{InsertAtEnd};
                if (index < InsertAtEnd)
                {
                    currentIndex = index + presetsAdded;
                }
                if (AddItem(entry.path().string(), currentIndex, allowDuplicates))
                {
                    presetsAdded++;
                }
            }
        }
    }

    return presetsAdded;
}


auto Playlist::RemoveItem(uint32_t index) -> bool
{
    if (index >= m_items.size())
    {
        return false;
    }

    m_items.erase(m_items.cbegin() + index);

    return true;
}


void Playlist::Shuffle(bool enabled)
{
    m_shuffle = enabled;
}


auto Playlist::Shuffle() const -> bool
{
    return m_shuffle;
}


void Playlist::Sort(uint32_t startIndex, uint32_t count,
                    Playlist::SortPredicate predicate, Playlist::SortOrder order)
{
    std::sort(m_items.begin() + startIndex,
              m_items.begin() + startIndex + count,
              [predicate, order](const Item& left, const Item& right) {
                  std::string leftFilename;
                  std::string rightFilename;

                  switch (predicate)
                  {
                      case SortPredicate::FullPath:
                          leftFilename = left.Filename();
                          rightFilename = right.Filename();
                          break;

                      case SortPredicate::FilenameOnly: {
                          leftFilename = path(left.Filename()).filename().string();
                          rightFilename = path(right.Filename()).filename().string();
                          break;
                      }
                  }

                  switch (order)
                  {
                      case SortOrder::Ascending:
                          return std::lexicographical_compare(leftFilename.begin(), leftFilename.end(),
                                                              rightFilename.begin(), rightFilename.end());
                      case SortOrder::Descending:
                          return std::lexicographical_compare(rightFilename.begin(), rightFilename.end(),
                                                              leftFilename.begin(), leftFilename.end());
                  }
              });
}


} // namespace Playlist
} // namespace ProjectM
