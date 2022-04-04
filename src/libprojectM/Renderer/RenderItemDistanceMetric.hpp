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

/// Compares two render items and returns zero if they are virtually equivalent and large values
/// when they are dissimilar. If two render items cannot be compared, NOT_COMPARABLE_VALUE is returned.
class RenderItemDistanceMetric : public std::binary_function<const RenderItem*, const RenderItem*, double>
{
public:
    virtual ~RenderItemDistanceMetric() = default;
    static double constexpr notComparableValue = 1.0;
    virtual auto operator()(const RenderItem* r1, const RenderItem* r2) const -> double = 0;
    virtual auto TypeIds() const -> TypeIdPair = 0;
};

// A base class to construct render item distance metrics. Just specify your two concrete
// render item types as template parameters and override the ComputeDistance() function.
template<class R1, class R2>
class RenderItemDistance : public RenderItemDistanceMetric
{

protected:
    // Override to create your own distance fmetric for your specified custom types.
    virtual auto ComputeDistance(const R1* r1, const R2* r2) const -> double = 0;

public:
    inline auto operator()(const RenderItem* r1, const RenderItem* r2) const -> double override
    {
        if (Supported(r1, r2))
        {
            return ComputeDistance(dynamic_cast<const R1*>(r1), dynamic_cast<const R2*>(r2));
        }
        if (Supported(r2, r1))
        {
            return ComputeDistance(dynamic_cast<const R1*>(r2), dynamic_cast<const R2*>(r1));
        }

        return notComparableValue;
    }

    // Returns true if and only if r1 and r2 are the same type as or derived from R1, R2 respectively
    inline auto Supported(const RenderItem* r1, const RenderItem* r2) const -> bool
    {
        return dynamic_cast<const R1*>(r1) && dynamic_cast<const R2*>(r2);
        //return typeid(r1) == typeid(const R1 *) && typeid(r2) == typeid(const R2 *);
    }

    inline auto TypeIds() const -> TypeIdPair override
    {
        return {typeid(const R1*).name(), typeid(const R2*).name()};
    }
};


class RTIRenderItemDistance : public RenderItemDistance<RenderItem, RenderItem>
{
public:
    ~RTIRenderItemDistance() override = default;

protected:
    inline auto ComputeDistance(const RenderItem* lhs, const RenderItem* rhs) const -> double override
    {
        if (typeid(*lhs) == typeid(*rhs))
        {
            //std::cerr << typeid(*lhs).name() << " and " << typeid(*rhs).name() <<  "are comparable" << std::endl;

            return 0.0;
        }

        //std::cerr << typeid(*lhs).name() << " and " << typeid(*rhs).name() <<  "not comparable" << std::endl;
        return notComparableValue;
    }
};


class ShapeXYDistance : public RenderItemDistance<Shape, Shape>
{

public:
    ~ShapeXYDistance() override = default;

protected:
    inline auto ComputeDistance(const Shape* lhs, const Shape* rhs) const -> double override
    {
        return (meanSquaredError(lhs->x, rhs->x) + meanSquaredError(lhs->y, rhs->y)) / 2;
    }
};


class MasterRenderItemDistance : public RenderItemDistance<RenderItem, RenderItem>
{
public:
    ~MasterRenderItemDistance() override = default;

protected:
    inline auto ComputeDistance(const RenderItem* lhs, const RenderItem* rhs) const -> double override
    {
        bool const bothShape = m_shapeXYDistance.Supported(lhs, rhs);

        return bothShape ? m_shapeXYDistance(lhs, rhs) : m_rttiDistance(lhs, rhs);
    }

private:
    RTIRenderItemDistance m_rttiDistance;
    ShapeXYDistance m_shapeXYDistance;
};

#endif /* RenderItemDISTANCEMETRIC_H_ */
