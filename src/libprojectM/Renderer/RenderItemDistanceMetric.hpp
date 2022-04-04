/*
 * RenderItemDistanceMetric.h
 *
 *  Created on: Feb 16, 2009
 *      Author: struktured
 */

#ifndef RenderItemDISTANCEMETRIC_H_
#define RenderItemDISTANCEMETRIC_H_

#include "Common.hpp"
#include "Shape.hpp"
#include <functional>
#include <limits>
#include <map>

using TypeIdPair = std::pair<std::string, std::string>;

namespace MasterRenderItemDistance {

double constexpr notComparableValue = 1.0;

/// Compares two render items and returns zero if they are virtually equivalent and large values
/// when they are dissimilar. If two render items cannot be compared, notComparableValue is returned.
inline auto Compute(const RenderItem* lhs, const RenderItem* rhs) -> double
{
    auto const rttiDistance = [lhs, rhs]() {
        if (typeid(*lhs) == typeid(*rhs))
        {
            return 0.0;
        }
        return notComparableValue;
    };

    auto const shapeDistance = [lhs, rhs]() {
        auto const* const lShape = dynamic_cast<Shape const*>(lhs);
        auto const* const rShape = dynamic_cast<Shape const*>(rhs);
        if ((lShape != nullptr) && (rShape != nullptr))
        {
            //CPP20: std::midpoint
            return (meanSquaredError(lShape->x, rShape->x) + meanSquaredError(lShape->y, rShape->y)) / 2;
        }

        return notComparableValue;
    };

    bool const bothShape =
        dynamic_cast<Shape const*>(lhs) != nullptr && dynamic_cast<Shape const*>(rhs) != nullptr;

    return bothShape ? shapeDistance() : rttiDistance();
}
}// namespace MasterRenderItemDistance

#endif /* RenderItemDISTANCEMETRIC_H_ */
