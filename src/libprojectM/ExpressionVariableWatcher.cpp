#include "ExpressionVariableWatcher.hpp"

#include "Utils.hpp"

#include <projectM-4/debug.h>

#include <cstdint>
#include <string>
#include <utility>

namespace libprojectM {

auto ExpressionVariableWatcher::Add(ExpressionBlocks block, uint32_t index, VariableName variableName) -> const ExpressionVariableWatcher::VariableValues*
{
    // Validate block value only, set index to 0 in blocks only occurring once.
    switch (block)
    {
        case PROJECTM_EXPR_PER_FRAME:
        case PROJECTM_EXPR_PER_POINT:
            index = 0;

        case PROJECTM_EXPR_SHAPE_PER_FRAME:
        case PROJECTM_EXPR_WAVE_PER_FRAME:
        case PROJECTM_EXPR_WAVE_PER_POINT:
        case PROJECTM_EXPR_MILKDROP_SPRITE:
            break;

        default:
            return nullptr;
    }

    libprojectM::Utils::ToLowerInPlace(variableName);

    IndexedExpressionBlock const indexedBlock = std::make_pair(block, index);

    auto blockInMap = m_watches.find(indexedBlock);
    if (blockInMap == m_watches.end())
    {
        blockInMap = m_watches.insert({indexedBlock, {}}).first;
    }

    // If the watch was already registered with the given, return the existing pointer again.
    auto variableWatch = blockInMap->second.find(variableName);
    if (variableWatch != blockInMap->second.end())
    {
        return &variableWatch->second;
    }

    // New watch. Create a new value struct and return it.
    return &blockInMap->second.insert({variableName, {}}).first->second;
}

auto ExpressionVariableWatcher::Remove(ExpressionBlocks block, uint32_t index, VariableName variableName) -> const ExpressionVariableWatcher::VariableValues*
{
    // Validate block value only, set index to 0 in blocks only occurring once.
    switch (block)
    {
        case PROJECTM_EXPR_PER_FRAME:
        case PROJECTM_EXPR_PER_POINT:
            index = 0;

        case PROJECTM_EXPR_SHAPE_PER_FRAME:
        case PROJECTM_EXPR_WAVE_PER_FRAME:
        case PROJECTM_EXPR_WAVE_PER_POINT:
        case PROJECTM_EXPR_MILKDROP_SPRITE:
            break;

        default:
            return nullptr;
    }

    libprojectM::Utils::ToLowerInPlace(variableName);

    IndexedExpressionBlock const indexedBlock = std::make_pair(block, index);

    auto blockInMap = m_watches.find(indexedBlock);
    if (blockInMap == m_watches.end())
    {
        return nullptr;
    }

    auto variableWatch = blockInMap->second.find(variableName);
    if (variableWatch == blockInMap->second.end())
    {
        return nullptr;
    }

    // Watch found, make a copy of the values struct pointer, then delete it.
    const auto* oldValuesStruct = &variableWatch->second;

    blockInMap->second.erase(variableWatch);

    return oldValuesStruct;
}

void ExpressionVariableWatcher::Clear()
{
    m_watches.clear();
}

auto ExpressionVariableWatcher::GetBlockWatches(ExpressionBlocks block, uint32_t index) -> BlockWatches&
{
    // Validate block value only.
    switch (block)
    {
        case PROJECTM_EXPR_PER_FRAME:
        case PROJECTM_EXPR_PER_POINT:
        case PROJECTM_EXPR_SHAPE_PER_FRAME:
        case PROJECTM_EXPR_WAVE_PER_FRAME:
        case PROJECTM_EXPR_WAVE_PER_POINT:
        case PROJECTM_EXPR_MILKDROP_SPRITE:
            break;

        default:
            throw InvalidBlockException("Block type " + std::to_string(block) + " is not supported!");
    }

    IndexedExpressionBlock const indexedBlock = std::make_pair(block, index);

    auto blockInMap = m_watches.find(indexedBlock);
    if (blockInMap == m_watches.end())
    {
        blockInMap = m_watches.insert({indexedBlock, {}}).first;
    }

    return blockInMap->second;
}

} // namespace libprojectM
