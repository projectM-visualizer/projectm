/*
 * RenderItemMergeFunction.hpp
 *
 *  Created on: Feb 16, 2009
 *      Author: struktured
 */

#ifndef RenderItemMergeFunction_HPP_
#define RenderItemMergeFunction_HPP_

#include "Common.hpp"
#include "Renderable.hpp"
#include <limits>
#include <functional>
#include <map>

/// Merges two render items and returns zero if they are virtually equivalent and large values
/// when they are dissimilar. If two render items cannot be compared, NOT_COMPARABLE_VALUE is returned.
class RenderItemMergeFunction {
public:
  virtual void operator()(const RenderItem * r1, const RenderItem * r2, RenderItem * r3, double ratio) const = 0;
  virtual TypeIdPair typeIdPair() const = 0;
};

/// A base class to construct render item distance mergeFunctions. Just specify your two concrete
/// render item types as template parameters and override the computeMerge() function.
template <class R1, class R2=R1, class R3=R2>
class RenderItemMerge : public RenderItemMergeFunction {

protected:
/// Override to create your own distance mergeFunction for your specified custom types.
virtual void computeMerge(const R1 * r1, const R2 * r2, R3 * r3, double ratio) const = 0;

public:

inline virtual void operator()(const RenderItem * r1, const RenderItem * r2, RenderItem * r3, double ratio) const {
	if (supported(r1, r2))
		return computeMerge(dynamic_cast<const R1*>(r1), dynamic_cast<const R2*>(r2), dynamic_cast<const R3*>(r3));
	else if (supported(r2,r1))
		return computeMerge(dynamic_cast<const R1*>(r2), dynamic_cast<const R2*>(r1), dynamic_cast<const R3*>(r3));
	else
		return;
}

/// Returns true if and only if r1 and r2 are of type R1 and R2 respectively.
inline bool supported(const RenderItem * r1, const RenderItem * r2) const {
	return typeid(r1) == typeid(const R1 *) && typeid(r2) == typeid(const R2 *);
}

inline TypeIdPair typeIdPair() const {
	return TypeIdPair(typeid(const R1*).name(), typeid(const R2*).name());
}

};


float interpolate(float a, float b, float ratio)
{
    return (ratio*a + (1-ratio)*b) * 0.5;
}

int interpolate(int a, int b, float ratio)
{
    return (int)(ratio*(float)a + (1-ratio)*(float)b) * 0.5;
}


/// EXAMPLE ONLY!!!
class ShapeXYMerge : public RenderItemMerge<Shape> {

public:

	ShapeXYMerge() {}
	virtual ~ShapeXYMerge() {}

protected:

	virtual inline void computeMerge(const Shape * lhs, const Shape * rhs, Shape * target, double ratio) const {

	    target->x = interpolate(lhs->x, rhs->x, ratio);
        target->y = interpolate(lhs->y, rhs->y, ratio);
	    target->a = interpolate(lhs->a, rhs->a, ratio);
        target->a2 = interpolate(lhs->a2, rhs->a2, ratio);
        target->r = interpolate(lhs->r, rhs->r, ratio);
        target->r2 = interpolate(lhs->r2, rhs->r2, ratio);
        target->g = interpolate(lhs->g, rhs->g, ratio);
        target->g2 = interpolate(lhs->g2, rhs->g2, ratio);
        target->b = interpolate(lhs->b, rhs->b, ratio);
        target->b2 = interpolate(lhs->b2, rhs->b2, ratio);

        target->ang = interpolate(lhs->ang, rhs->ang, ratio);
        target->radius = interpolate(lhs->radius, rhs->radius, ratio);

        target->tex_ang = interpolate(lhs->tex_ang, rhs->tex_ang, ratio);
        target->tex_zoom = interpolate(lhs->tex_zoom, rhs->tex_zoom, ratio);

        target->border_a = interpolate(lhs->border_a, rhs->border_a, ratio);
        target->border_r = interpolate(lhs->border_r, rhs->border_r, ratio);
        target->border_g = interpolate(lhs->border_g, rhs->border_g, ratio);
        target->border_b = interpolate(lhs->border_b, rhs->border_b, ratio);

        target->sides = interpolate(lhs->sides, rhs->sides, ratio);

        target->additive = interpolate(lhs->additive, rhs->additive, ratio);
        target->textured = interpolate(lhs->textured, rhs->textured, ratio);
        target->thickOutline = interpolate(lhs->thickOutline, rhs->thickOutline, ratio);

        target->masterAlpha = interpolate(lhs->masterAlpha, rhs->masterAlpha, ratio);
        target->imageUrl

	    return;
	}

};


/// Use as the top level merge function. It stores a map of all other
/// merge functions, using the function that fits best with the
/// incoming type parameters.
class MasterRenderItemMerge : public RenderItemMerge<RenderItem> {

typedef std::map<TypeIdPair, RenderItemMergeFunction*> MergeFunctionMap;
public:

	MasterRenderItemMerge() {}
	virtual ~MasterRenderItemMerge() {}

	inline void add(RenderItemMergeFunction * fun) {
		_mergeFunctionMap[fun->typeIdPair()] = fun;
	}

protected:
	virtual inline void computeMerge(const RenderItem * lhs, const RenderItem * rhs, RenderItem * renderItem, double ratio) const {

		RenderItemMergeFunction * mergeFunction;

		TypeIdPair pair(typeid(lhs), typeid(rhs));
		if (_mergeFunctionMap.count(pair)) {
			mergeFunction = _mergeFunctionMap[pair];
		} else if (_mergeFunctionMap.count(pair = TypeIdPair(typeid(rhs), typeid(lhs)))) {
			mergeFunction = _mergeFunctionMap[pair];
		} else {
			mergeFunction  = 0;
		}

		// If specialized mergeFunction exists, use it to get higher granularity
		// of correctness
		if (mergeFunction)
			renderItem = (*mergeFunction)(lhs, rhs, ratio);
		else
			renderItem = 0;
	}

private:
	mutable MergeFunctionMap _mergeFunctionMap;
};

#endif /* RenderItemMergeFunction_HPP_ */
