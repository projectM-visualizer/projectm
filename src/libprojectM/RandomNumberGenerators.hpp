#ifndef RANDOM_NUMBER_GENERATORS_HPP
#define RANDOM_NUMBER_GENERATORS_HPP
#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>

#define WEIGHTED_RANDOM_DEBUG 0

namespace RandomNumberGenerators {

inline double uniform()
/* Uniform random number generator x(n+1)= a*x(n) mod c
				with a = pow(7,5) and c = pow(2,31)-1.
				Copyright (c) Tao Pang 1997. */
	{
		const int ia=16807,ic=2147483647,iq=127773,ir=2836;
		int il,ih,it;
		double rc;
		static int iseed = rand();
		ih = iseed/iq;
		il = iseed%iq;
		it = ia*il-ir*ih;
		if (it > 0)
		{
			iseed = it;
		}
		else
		{
			iseed = ic+it;
		}
		rc = ic;
		return iseed/rc;
	}

inline double gaussian(double mean, double sigma)
{

    double x1, x2, w, y1;
 
	do {
		x1 = 2.0 * uniform() - 1.0;
		x2 = 2.0 * uniform() - 1.0;
		w = x1 * x1 + x2 * x2;
	} while ( w >= 1.0 );

	w = sqrt( (-2.0 * log( w ) ) / w );
	y1 = x1 * w;

    double ret = y1*sigma + mean;

	return ret;
}

inline std::size_t uniformInteger(std::size_t upperBound=1) {

	
	/// @bug there was a man entry about how this leads to a lousy uniform
	/// @bug distribution in practice. should probably review
	assert(upperBound > 0);
	return ((rand()) % ((int)upperBound));
}

	
	

/// Randomizes from probabilistically weighted distribution. Thus,
/// sum of passed in weights should be 1.0
inline std::size_t weightedRandomNormalized(std::vector<float> weights) {

        // Choose a random bounded mass between 0 and 1
	float cutoff = ((float)(rand())) / (float)RAND_MAX;

	//std::cout << "cutoff : " << cutoff << std::endl;

    // Sum up mass, stopping when cutoff is reached. This is the typical
    // weighted sampling algorithm.
	float mass = 0;
	for (std::size_t i = 0; i< weights.size() ; i++) {
		mass += weights[i];
		//std::cout << "mass: " << mass << std::endl;
		if (mass >= cutoff)
			return i;
	}

    // Just in case something slips through the cracks
	return weights.size()-1;
}

inline std::size_t weightedRandom(const std::vector<int> & weights, unsigned int weightTotalHint = 0) {
	

	if (weightTotalHint == 0)	{		
		for (std::size_t i = 0; i < weights.size();i++) 
			weightTotalHint += weights[i];
	}
	
	const int sampledSum = uniformInteger(weightTotalHint);
	int sum = 0;
	if (WEIGHTED_RANDOM_DEBUG) std::cout << "[RNG::weightedRandom()] weightTotal = " << weightTotalHint <<
			 std::endl; 

	for (std::size_t i = 0; i < weights.size();i++) {
		if (WEIGHTED_RANDOM_DEBUG) 
		std::cout << "[RNG::weightedRandom()] weight[" << i << "] = " << weights[i] <<
			 std::endl; 

		sum += weights[i];
		if (sampledSum <= sum) {
			if (WEIGHTED_RANDOM_DEBUG) 
			std::cout << "[RNG::weightedRandom()] sampled index " << i << "(" <<
			 "running sum = " << sum << ", sampled sum = " << sampledSum << std::endl;
			return i;
		}
	}

	return weights.size()-1;
}

}
#endif
