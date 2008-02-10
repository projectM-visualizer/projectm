#fndef RANDOM_NUMBER_GENERATORS_HPP
#define RANDOM_NUMBER_GENERATORS_HPP
#include <cmath>

namespace RandomNumberGenerators {

double gaussian( double min = 0, double max=1, double sigma=1, double mean=.5)
{
	double random = (min + (max-min) * (double)rand()/RAND_MAX); //create random domain between [min,max]
	double tmp = (random-mean)/sigma;
	double gauss= exp(-tmp*tmp/2); //gaussian formula
	
	return gauss;
}
	
	
}

#endif