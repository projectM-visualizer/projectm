#include "Playlist.hpp"

#include <algorithm>

// Fall back to boost if compiler doesn't support C++17
#include FS_INCLUDE
using namespace FS_NAMESPACE::filesystem;

namespace ProjectM {
namespace Playlist {

const char* PlaylistEmptyException::what() const noexcept
{
    return "Playlist is empty";
}

Playlist::Playlist()
{
    m_randomGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}


uint32_t Playlist::Size() const
{
    return m_items.size();
}


bool Playlist::Empty() const
{
    return m_items.empty();
}


void Playlist::Clear()
{
    m_presetHistory.clear();
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

    if (!m_filter.Passes(filename))
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

    m_presetHistory.clear();
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

    m_presetHistory.clear();
    if (recursive)
    {
        try
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
        catch (std::exception&)
        {
            // Todo: Add failure feedback
            return presetsAdded;
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

    m_presetHistory.clear();
    m_items.erase(m_items.cbegin() + index);

    return true;
}


void Playlist::SetShuffle(bool enabled)
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
    if (startIndex >= m_items.size())
    {
        return;
    }

    if (startIndex + count >= m_items.size())
    {
        count = m_items.size() - startIndex;
    }

    m_presetHistory.clear();

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

                  return false;
              });
}


auto Playlist::NextPresetIndex() -> size_t
{
    if (m_items.empty())
    {
        throw PlaylistEmptyException();
    }

    AddCurrentPresetIndexToHistory();

    if (m_shuffle)
    {
        std::uniform_int_distribution<size_t> randomDistribution(0, m_items.size());
        m_currentPosition = randomDistribution(m_randomGenerator);
    }
    else
    {
        m_currentPosition++;
        if (m_currentPosition >= m_items.size())
        {
            m_currentPosition = 0;
        }
    }

    return m_currentPosition;
}


auto Playlist::PreviousPresetIndex() -> size_t
{
    if (m_items.empty())
    {
        throw PlaylistEmptyException();
    }

    AddCurrentPresetIndexToHistory();

    if (m_shuffle)
    {
        std::uniform_int_distribution<size_t> randomDistribution(0, m_items.size());
        m_currentPosition = randomDistribution(m_randomGenerator);
    }
    else
    {
        if (m_currentPosition == 0)
        {
            m_currentPosition = m_items.size() - 1;
        }
        else
        {
            m_currentPosition--;
        }
    }

    return m_currentPosition;
}

auto Playlist::LastPresetIndex() -> size_t
{
    if (m_items.empty())
    {
        throw PlaylistEmptyException();
    }

    if (!m_presetHistory.empty())
    {
        m_currentPosition = m_presetHistory.back();
        m_presetHistory.pop_back();
    }
    else
    {
        m_currentPosition = PreviousPresetIndex();
        // Remove added history item again to prevent ping-pong behavior
        m_presetHistory.pop_back();
    }

    return m_currentPosition;
}


auto Playlist::PresetIndex() const -> size_t
{
    if (m_items.empty())
    {
        throw PlaylistEmptyException();
    }

    return m_currentPosition;
}


auto Playlist::SetPresetIndex(size_t presetIndex) -> size_t
{
    if (m_items.empty())
    {
        throw PlaylistEmptyException();
    }

    AddCurrentPresetIndexToHistory();

    if (presetIndex == m_currentPosition)
    {
        return m_currentPosition;
    }

    m_currentPosition = presetIndex;

    if (m_currentPosition >= m_items.size())
    {
        m_currentPosition = 0;
    }

    return m_currentPosition;
}


void Playlist::RemoveLastHistoryEntry()
{
    if (!m_presetHistory.empty())
    {
        m_presetHistory.pop_back();
    }
}


auto Playlist::Filter() -> class Filter&
{
    return m_filter;
}


auto Playlist::ApplyFilter() -> size_t
{
    size_t itemsRemoved{};

    for (auto it = begin(m_items); it != end(m_items);)
    {
        if (!m_filter.Passes(it->Filename()))
        {
            it = m_items.erase(it);
            itemsRemoved++;
        }
        else
        {
            ++it;
        }
    }

    if (itemsRemoved != 0)
    {
        m_presetHistory.clear();
    }

    return itemsRemoved;
}


void Playlist::AddCurrentPresetIndexToHistory()
{
    // No duplicate entries.
    if (!m_presetHistory.empty() && m_currentPosition == m_presetHistory.back())
    {
        return;
    }

    m_presetHistory.push_back(m_currentPosition);
    if (m_presetHistory.size() > MaxHistoryItems)
    {
        m_presetHistory.pop_front();
    }
}


} // namespace Playlist
} // namespace ProjectM
