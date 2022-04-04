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
    bool const sameType = typeid(*lhs) == typeid(*rhs);

    auto const rttiDistance = [sameType]() {
        return sameType ? 0.0 : notComparableValue;
    };

    auto const* const lShape = dynamic_cast<Shape const*>(lhs);
    auto const* const rShape = dynamic_cast<Shape const*>(rhs);
    bool const bothShape = lShape != nullptr && rShape != nullptr;

    auto const shapeDistance = [lShape, rShape, bothShape]() {
        //CPP20: std::midpoint
        return bothShape
            ? (meanSquaredError(lShape->x, rShape->x) + meanSquaredError(lShape->y, rShape->y)) / 2
            : notComparableValue;
    };

    return sameType && bothShape ? shapeDistance() : rttiDistance();
}
}// namespace MasterRenderItemDistance

#endif /* RenderItemDISTANCEMETRIC_H_ */
