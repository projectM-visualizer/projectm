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

typedef std::vector<std::pair<const RenderItem*, const RenderItem*> > RenderItemMatchList;

class MatchResults;

class RenderItemMatcher : public std::binary_function<RenderItemList, RenderItemList, MatchResults> {

public:

struct MatchResults {
  RenderItemMatchList matches;
  double error;
};

	static const std::size_t MAXIMUM_SET_SIZE = 1000;

	/// Computes an optimal matching between two renderable item sets.
	/// @param lhs the "left-hand side" list of render items.
	/// @param rhs the "right-hand side" list of render items.
	/// @returns a list of match pairs, possibly self referencing, and an error estimate of the matching.
	inline virtual MatchResults operator()(const RenderItemList & lhs, const RenderItemList & rhs) const {
		
		 MatchResults results;

		// Ensure the first argument is greater than next to aid the helper function's logic.
		if (lhs.size() >= rhs.size()) {
		  results.error = computeMatching(lhs, rhs);
		  setMatches(results.matches, lhs, rhs);
		} else {
		  results.error = computeMatching(rhs, lhs);
		  setMatches(results.matches, rhs, lhs);
		}
		return results;
	
	}

	RenderItemMatcher() {}
	virtual ~RenderItemMatcher() {}

	MasterRenderItemDistance & distanceFunction() { return _distanceFunction; }

private:
	mutable HungarianMethod<MAXIMUM_SET_SIZE> _hungarianMethod;
	mutable double _weights[MAXIMUM_SET_SIZE][MAXIMUM_SET_SIZE];


	/// @idea interface this entirely allow overriding of its type.
	mutable MasterRenderItemDistance _distanceFunction;

	double computeMatching(const RenderItemList & lhs, const RenderItemList & rhs) const;

	void setMatches(RenderItemMatchList & dest, const RenderItemList & lhs_src, const RenderItemList & rhs_src) const;

};

#endif
