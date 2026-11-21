/*
* projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
#pragma once

#include <projectM-4/debug.h>

#include <cstdint>
#include <exception>
#include <map>
#include <string>
#include <utility>

namespace libprojectM {

/**
 * @class ExpressionVariableWatcher
 * @brief Container class managing active expression variable watches.
 *
 * All watches are stored in a three-tiered map, with the first map using the block and index as key.
 * The second level map associated each watched variable to the struct which will receive the
 * values. This struct and is also used by the outside application to display or otherwise evaluate
 * the values after a frame is rendered.
 */
class ExpressionVariableWatcher
{
public:
    /**
     * @brief Exception for an invalid block argument in GetBlockWatches().
     */
    class InvalidBlockException : public std::exception
    {
    public:
        InvalidBlockException(std::string message)
            : m_message(std::move(message))
        {
        }

        ~InvalidBlockException() override = default;

        auto what() const noexcept -> const char* override
        {
            return m_message.c_str();
        }

        auto message() const -> const std::string&
        {
            return m_message;
        }

    private:
        std::string m_message;
    };

    using ExpressionBlocks = projectm_expression_blocks;                  //!< Alias for the projectm_expression_blocks public API enum.
    using IndexedExpressionBlock = std::pair<ExpressionBlocks, uint32_t>; //!< A pair if ExpressionBlocks and the custom shape/waveform or sprite index.
    using VariableName = std::string;                                     //!< the name of a watched variable, lower-case.
    using VariableValues = projectm_expression_variable_values;           //!< Alias for the projectm_expression_variable_values public API struct.
    using BlockWatches = std::map<VariableName, VariableValues>;          //!< A map of variable names to the associated value structs.

    /**
     * @brief Adds a new watch, or returns a pointer to the previously registered one.
     * @param block The expression code block to add the watch for.
     * @param index The custom shape/waveform or user sprite index.
     * @param variableName The variable to watch.
     * @return A pointer to the newly added values struct, if the watch was successfully added.
     */
    auto Add(ExpressionBlocks block, uint32_t index, VariableName variableName) -> const VariableValues*;

    /**
     * @brief Removes a previously registered variable watch.
     * @param block The expression code block to remove the watch from.
     * @param index The custom shape/waveform or user sprite index.
     * @param variableName The variable to unwatch.
     * @return If the previous watch was found, a pointer to the initially registered values struct,
     *         or nullptr if no watch was found.
     */
    auto Remove(ExpressionBlocks block, uint32_t index, VariableName variableName) -> const VariableValues*;

    /**
     * Clears all previously registered watches.
     */
    void Clear();

    /**
     * @brief Returns all watched variables and their value structs for the given block and index.
     * @throws InvalidBlockException Thrown if the passed block value is invalid.
     * @param block The expression code block to return the watch list for.
     * @param index The custom shape/waveform or user sprite index to return the watch list for.
     * @return A reference to a map of type BlockWatches with all registered watched variables.
     */
    auto GetBlockWatches(ExpressionBlocks block, uint32_t index) -> BlockWatches&;

private:
    /**
     * @brief Registered watches.
     *
     * A three-tiered map of indexed blocks, variables and the value struct returned to the application.
     */
    std::map<IndexedExpressionBlock, BlockWatches> m_watches;
};

} // namespace libprojectM
