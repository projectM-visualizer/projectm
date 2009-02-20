/*
 * RenderItemMatcher.hpp
 *
 *  Created on: Feb 16, 2009
 *      Author: struktured
 */

#ifndef RenderItemMatcher_HPP
#define RenderItemMatcher_HPP

#include "RenderItemDistanceMetric.hpp"
#include <vector>
#include <map>
#include <iostream>
#include "HungarianMethod.hpp"


class RenderItemMatcher : public std::binary_function<RenderItemList, RenderItemList, double> {
public:
	static const std::size_t MAXIMUM_SET_SIZE = 1000;

	/// Computes an optimal matching between two renderable item sets.
	inline virtual double operator()(const RenderItemList & lhs, const RenderItemList & rhs) const {

		// Ensure the first argument is greater than next to aid the helper function's logic.
		if (lhs.size() >= rhs.size())
			return computeMatching(lhs, rhs);
		else
			return computeMatching(rhs, lhs);
	}

	/// @idea could instead just pass back a vector of render item pointer pairs. ask sperl
	inline int matching(int i) const { return _hungarianMethod.matching(i); }
	inline int inverseMatching(int j) const { return _hungarianMethod.inverseMatching(j); }
	inline double weight(int i, int j) const { return _weights[i][j]; }

	RenderItemMatcher() {}
	virtual ~RenderItemMatcher() {}

	MasterRenderItemDistance & distanceFunction() { return _distanceFunction; }

private:
	mutable HungarianMethod<MAXIMUM_SET_SIZE> _hungarianMethod;
	mutable double _weights[MAXIMUM_SET_SIZE][MAXIMUM_SET_SIZE];

	/// @idea interface this entirely allow overriding of its type.
	mutable MasterRenderItemDistance _distanceFunction;

	inline double computeMatching(const RenderItemList & lhs, const RenderItemList & rhs) const {
		for (int i = 0; i < lhs.size();i++) {
			int j;
			for (j = 0; j < rhs.size();j++)
				_weights[i][j] = _distanceFunction(lhs[i], rhs[j]);
			for (; j < lhs.size();j++)
				_weights[i][j] = RenderItemDistanceMetric::NOT_COMPARABLE_VALUE;
		}

		const double error = _hungarianMethod(_weights, lhs.size());
		std::cout << "[computeMatching] total error is " << error << std::endl;
		return error;
	}

};

#endif
