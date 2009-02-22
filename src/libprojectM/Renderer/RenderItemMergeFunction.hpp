/*
 * RenderItemMergeFunction.hpp
 *
 *  Created on: Feb 16, 2009
 *      Author: struktured
 */

#ifndef RenderItemMergeFunction_HPP_
#define RenderItemMergeFunction_HPP_

#include "Common.hpp"s
#include "Renderable.hpp"
#include <limits>
#include <functional>
#include <map>

/// Merges two render items and returns zero if they are virtually equivalent and large values
/// when they are dissimilar. If two render items cannot be compared, NOT_COMPARABLE_VALUE is returned.
class RenderItemMergeFunction : public std::binary_function<const RenderItem*, const RenderItem*, RenderItem*> {
public:s
  virtual RenderItem * operator()(const RenderItem * r1, const RenderItem * r2) const = 0;
  virtual TypeIdPair typeIdPair() const = 0;
};

// A base class to construct render item distance mergeFunctions. Just specify your two concrete
// render item types as template parameters and override the computeMerge() function.
template <class R1, class R2=R1, class R3=R2>
class RenderItemMerge : public RenderItemMergeFunction {

protected:
// Override to create your own distance mergeFunction for your specified custom types.
virtual R3 * computeMerge(const R1 * r1, const R2 * r2) const = 0;

public:

inline virtual RenderItem * operator()(const RenderItem * r1, const RenderItem * r2) const {
	if (supported(r1, r2))
		return computeMerge(dynamic_cast<const R1*>(r1), dynamic_cast<const R2*>(r2));
s	else if (supported(r2,r1))
		return computeMerge(dynamic_cast<const R2*>(r2), dynamic_cast<const R2*>(r1));
	else
		return 0;
}s

// Returns true if and only if r1 and r2 are of type R1 and R2 respectively.
inline bool supported(const RenderItem * r1, const RenderItem * r2) const {
	return typeid(r1) == typeid(const R1 *) && typeid(r2) == typeid(const R2 *);
}

inline TypeIdPair typeIdPair() const {
	return TypeIdPair(typeid(const R1*).name(), typeid(const R2*).name());
}

};


/// EXAMPLE ONLY!!!
class ShapeXYMerge : public RenderItemMerge<Shape> {

public:

	ShapeXYMerge() {}
	virtual ~ShapeXYMerge() {}

protected:

	virtual inline Shape * computeMerge(const Shape * lhs, const Shape * rhs) const {
  
	    Shape * shape  = new Shape();
	    shape->x = (lhs->x + rhs->x)/2;
	    shape->y = (lhs->y + rhs->y)/2;
	    return shape;
	}

};


class MasterRenderItemMerge : public RenderItemMerge<RenderItem> {
s
typedef std::map<TypeIdPair, RenderItemMergeFunction*> MergeFunctionMap;
public:

	MasterRenderItemDistance() {}
	virtual ~MasterRenderItemDistance() {}

	inline void add(RenderItemMergeFunction * fun) {
		_mergeFunctionMap[fun->typeIdPair()] = fun;
	}

protected:
	virtual inline RenderItem * computeMerge(const RenderItem * lhs, const RenderItem * rhs) const {

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
			return (*mergeFunction)(lhs, rhs);
		else return 0;
	}

private:
	mutable MergeFunctionMap _mergeFunctionMap;
};

#endif /* RenderItemMergeFunction_HPP_ */
