/*
 * RenderItemDistanceMetric.h
 *
 *  Created on: Feb 16, 2009
 *      Author: struktured
 */

#ifndef RenderItemDISTANCEMETRIC_H_
#define RenderItemDISTANCEMETRIC_H_

#include "Common.hpp"
#include "Renderable.hpp"
#include <limits>
#include <functional>
#include <map>


/// Compares two render items and returns zero if they are virtually equivalent and large values
/// when they are dissimilar. If two render items cannot be compared, NOT_COMPARABLE_VALUE is returned.
class RenderItemDistanceMetric : public std::binary_function<const RenderItem*, const RenderItem*, double> {
public:
  const static double NOT_COMPARABLE_VALUE;
  virtual double operator()(const RenderItem * r1, const RenderItem * r2) const = 0;
  virtual TypeIdPair typeIdPair() const = 0;
};

// A base class to construct render item distance metrics. Just specify your two concrete
// render item types as template parameters and override the computeDistance() function.
template <class R1, class R2>
class RenderItemDistance : public RenderItemDistanceMetric {

protected:
// Override to create your own distance metric for your specified custom types.
virtual double computeDistance(const R1 * r1, const R2 * r2) const = 0;

public:

inline virtual double  operator()(const RenderItem * r1, const RenderItem * r2) const {
	if (supported(r1, r2))
		return computeDistance(dynamic_cast<const R1*>(r1), dynamic_cast<const R2*>(r2));
	else if (supported(r2,r1))
		return computeDistance(dynamic_cast<const R2*>(r2), dynamic_cast<const R1*>(r1));
	else
		return NOT_COMPARABLE_VALUE;
}

// Returns true if and only if r1 and r2 are of type R1 and R2 respectively.
inline bool supported(const RenderItem * r1, const RenderItem * r2) const {
	return typeid(r1) == typeid(const R1 *) && typeid(r2) == typeid(const R2 *);
}

inline TypeIdPair typeIdPair() const {
	return TypeIdPair(typeid(const R1*).name(), typeid(const R2*).name());
}

};


class RTIRenderItemDistance : public RenderItemDistance<RenderItem, RenderItem> {
public:

	RTIRenderItemDistance() {}
	virtual ~RTIRenderItemDistance() {}

protected:
	virtual inline double computeDistance(const RenderItem * lhs, const RenderItem * rhs) const {
		if (typeid(lhs) == typeid(rhs))
			return 0.0;
		else
			return NOT_COMPARABLE_VALUE;
	}


};



class ShapeXYDistance : public RenderItemDistance<Shape, Shape> {

public:

	ShapeXYDistance() {}
	virtual ~ShapeXYDistance() {}

protected:

	virtual inline double computeDistance(const Shape * lhs, const Shape * rhs) const {
			return (meanSquaredError(lhs->x, rhs->x) + meanSquaredError(lhs->y, rhs->y)) / 2;
	}

};


class MasterRenderItemDistance : public RenderItemDistance<RenderItem, RenderItem> {

typedef std::map<TypeIdPair, RenderItemDistanceMetric*> DistanceMetricMap;
public:

	MasterRenderItemDistance() {}
	virtual ~MasterRenderItemDistance() {}

	inline void addMetric(RenderItemDistanceMetric * fun) {
		_distanceMetricMap[fun->typeIdPair()] = fun;
	}

protected:
	virtual inline double computeDistance(const RenderItem * lhs, const RenderItem * rhs) const {

		RenderItemDistanceMetric * metric;

		TypeIdPair pair(typeid(lhs), typeid(rhs));
		if (_distanceMetricMap.count(pair)) {
			metric = _distanceMetricMap[pair];
		} else if (_distanceMetricMap.count(pair = TypeIdPair(typeid(rhs), typeid(lhs)))) {
			metric = _distanceMetricMap[pair];
		} else {
			metric  = 0;
		}

		// If specialized metric exists, use it to get higher granularity
		// of correctness
		if (metric)
			return (*metric)(lhs, rhs);
		else // Failing that, use rtti and return 0 if they match, max value otherwise
			return _rttiDistance(lhs,rhs);
	}

private:
	mutable RTIRenderItemDistance _rttiDistance;
	mutable DistanceMetricMap _distanceMetricMap;
};

#endif /* RenderItemDISTANCEMETRIC_H_ */
