#pragma once

#include "Filter.hpp"
#include "Item.hpp"

#include <cstdint>
#include <limits>
#include <list>
#include <random>
#include <string>
#include <vector>

namespace libprojectM {
namespace Playlist {


class PlaylistEmptyException : public std::exception
{
public:

    const char* what() const noexcept override;
};



class Playlist
{
public:

    static constexpr auto InsertAtEnd = std::numeric_limits<uint32_t>::max();


    static constexpr size_t MaxHistoryItems = 1000;


    enum class SortPredicate
    {
        FullPath,
        FilenameOnly
    };


    enum class SortOrder
    {
        Ascending,
        Descending
    };


    Playlist();


    virtual ~Playlist() = default;


    virtual uint32_t Size() const;


    virtual bool Empty() const;


    virtual void Clear();


    virtual const std::vector<Item>& Items() const;


    virtual auto AddItem(const std::string& filename, uint32_t index, bool allowDuplicates) -> bool;


    virtual auto AddPath(const std::string& path, uint32_t index, bool recursive,
                         bool allowDuplicates) -> uint32_t;


    virtual auto RemoveItem(uint32_t index) -> bool;


    virtual void SetShuffle(bool enabled);


    virtual auto Shuffle() const -> bool;


    virtual void Sort(uint32_t startIndex, uint32_t count, SortPredicate predicate, SortOrder order);


    virtual auto NextPresetIndex() -> uint32_t;


    virtual auto PreviousPresetIndex() -> uint32_t;


    virtual auto LastPresetIndex() -> uint32_t;


    virtual auto PresetIndex() const -> uint32_t;


    virtual auto SetPresetIndex(uint32_t presetIndex) -> uint32_t;


    virtual void RemoveLastHistoryEntry();


    virtual auto Filter() -> class Filter&;


    virtual auto ApplyFilter() -> uint32_t;

private:

    void AddCurrentPresetIndexToHistory();

    std::vector<Item> m_items;
    class Filter m_filter;
    bool m_shuffle{false};
    uint32_t m_currentPosition{0};
    std::list<uint32_t> m_presetHistory;

    std::default_random_engine m_randomGenerator;
};

}
}
