#ifndef RANDOM_NUMBER_GENERATORS_HPP
#define RANDOM_NUMBER_GENERATORS_HPP
#include <cmath>
#include <vector>
#include <cassert>
namespace RandomNumberGenerators {

inline float uniform()
/* Uniform random number generator x(n+1)= a*x(n) mod c
				with a = pow(7,5) and c = pow(2,31)-1.
				Copyright (c) Tao Pang 1997. */
	{
		const int ia=16807,ic=2147483647,iq=127773,ir=2836;
		int il,ih,it;
		float rc;
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

inline float gaussian(float mean, float sigma)
{

	float x1, x2, w, y1, y2;
 
	do {
		x1 = 2.0 * uniform() - 1.0;
		x2 = 2.0 * uniform() - 1.0;
		w = x1 * x1 + x2 * x2;
	} while ( w >= 1.0 );

	w = sqrt( (-2.0 * log( w ) ) / w );
	y1 = x1 * w;
	y2 = x2 * w;
	
	float ret = y1*sigma + mean;

	return ret;
}

inline std::size_t uniformInteger(std::size_t upperBound=1) {

	
	/// @bug there was a man entry about how this leads to a lousy uniform
	/// @bug distribution in practice. should probably review
	assert(upperBound > 0);
	return ((rand()) % ((int)upperBound));
}

	
inline std::size_t weightedRandom(const std::vector<int> & weights, int weightTotalHint = 0) {
	
	if (weightTotalHint == 0)		
		for (std::size_t i = 0; i < weights.size();i++) {
			weightTotalHint += weights[i];
		}
		
	int sampledSum = uniformInteger(weightTotalHint);
	int sum = 0;
	
	for (std::size_t i = 0; i < weights.size();i++) {
		sum += weights[i];
		if (sampledSum <= sum)
			return i;
	}
	return weights.size()-1;
}

}
#endif
