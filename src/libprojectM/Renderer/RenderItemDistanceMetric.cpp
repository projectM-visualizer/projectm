/*
 * RenderItemDistanceMetric.cpp
 *
 *  Created on: Feb 18, 2009
 *      Author: struktured
 */

#include "RenderItemDistanceMetric.hpp"

// Assumes [0, 1] distance space because it's easy to manage with overflow
// Underflow is obviously possible though.
const double RenderItemDistanceMetric::notComparableValue(1.0);
inline auto RTIRenderItemDistance::ComputeDistance(const RenderItem* lhs, const RenderItem* rhs) const -> double
{
    if (typeid(*lhs) == typeid(*rhs))
    {
        //std::cerr << typeid(*lhs).name() << " and " << typeid(*rhs).name() <<  "are comparable" << std::endl;

        return 0.0;
    }
    else
    {
        //std::cerr << typeid(*lhs).name() << " and " << typeid(*rhs).name() <<  "not comparable" << std::endl;
        return notComparableValue;
    }
}
