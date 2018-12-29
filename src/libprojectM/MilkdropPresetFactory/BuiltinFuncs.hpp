//
// C++ Interface: BuiltinFuncs
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _BUILTIN_FUNCS_HPP
#define _BUILTIN_FUNCS_HPP

#include "Common.hpp"
#include "Func.hpp"
#include <cmath>
#include <cstdlib>
#include <cassert>

#include "RandomNumberGenerators.hpp"
#include "Expr.hpp"

/* Wrappers for all the builtin functions
   The arg_list pointer is a list of floats. Its
   size is equal to the number of arguments the parameter
   takes */
class FuncWrappers {

/* Values to optimize the sigmoid function */
static const int R =  32767;
static const int RR = 65534;

public:

static inline expr_t int_wrapper(expr_t * arg_list) {

return floor(arg_list[0]);

}


static inline expr_t sqr_wrapper(expr_t * arg_list) {
	return pow(arg_list[0], 2);
}


static inline expr_t sigmoid_wrapper(expr_t * arg_list)
{
	const double t = (1+exp(-arg_list[0]*arg_list[1]));
	return (fabs(t) > 0.00001) ? 1.0/t : 0;
}

static inline expr_t sign_wrapper(expr_t * arg_list) {

return -arg_list[0];
}

static inline expr_t min_wrapper(expr_t * arg_list) {

if (arg_list[0] > arg_list[1])
return arg_list[1];

return arg_list[0];
}

static inline expr_t max_wrapper(expr_t * arg_list) {

if (arg_list[0] > arg_list[1])
return arg_list[0];

return arg_list[1];
}


static inline expr_t bor_wrapper(expr_t * arg_list) {

return (expr_t)((int)arg_list[0] || (int)arg_list[1]);
}

static inline expr_t band_wrapper(expr_t * arg_list) {
return (expr_t)((int)arg_list[0] && (int)arg_list[1]);
}

static inline expr_t bnot_wrapper(expr_t * arg_list) {
return (expr_t)(!(int)arg_list[0]);
}

static inline expr_t if_wrapper(expr_t * arg_list) {

	if ((int)arg_list[0] == 0)
		return arg_list[2];
	//std::cout <<"NOT ZERO: " << arg_list[0] << std::endl;
	return arg_list[1];
}


static inline expr_t rand_wrapper(expr_t * arg_list) {
expr_t l=1;

//  printf("RAND ARG:(%d)\n", (int)arg_list[0]);
if ((int)arg_list[0] > 0)
	l  = (expr_t) RandomNumberGenerators::uniformInteger((int)arg_list[0]);

return l;
}

static inline expr_t equal_wrapper(expr_t * arg_list) {
	return (arg_list[0] == arg_list[1]);
}


static inline expr_t above_wrapper(expr_t * arg_list) {

return (arg_list[0] > arg_list[1]);
}


static inline expr_t below_wrapper(expr_t * arg_list) {

return (arg_list[0] < arg_list[1]);
}

static expr_t sin_wrapper(expr_t * arg_list) {
	const expr_t d = sin(arg_list[0]);
	return d;
}


static inline expr_t cos_wrapper(expr_t * arg_list) {
return (cos (arg_list[0]));
}

static inline expr_t tan_wrapper(expr_t * arg_list) {
return (tan(arg_list[0]));
}

static inline expr_t asin_wrapper(expr_t * arg_list) {
return (asin (arg_list[0]));
}

static inline expr_t acos_wrapper(expr_t * arg_list) {
return (acos (arg_list[0]));
}

static inline expr_t atan_wrapper(expr_t * arg_list) {
return (atan (arg_list[0]));
}

static inline expr_t atan2_wrapper(expr_t * arg_list) {
return (atan2 (arg_list[0], arg_list[1]));
}

static inline expr_t pow_wrapper(expr_t * arg_list) {
return (pow (arg_list[0], arg_list[1]));
}

static inline expr_t exp_wrapper(expr_t * arg_list) {
return (exp(arg_list[0]));
}

static inline expr_t abs_wrapper(expr_t * arg_list) {
return (fabs(arg_list[0]));
}

static inline expr_t log_wrapper(expr_t* arg_list) {
return (log (arg_list[0]));
}

static inline expr_t log10_wrapper(expr_t * arg_list) {
return (log10 (arg_list[0]));
}

static inline expr_t sqrt_wrapper(expr_t * arg_list) {
return (sqrt (arg_list[0]));
}


static inline expr_t print_wrapper(expr_t * arg_list) {

	int len  = 1;
	for (int i = 0; i < len; i++)
	{
		std::cout << arg_list[i];
		if (i != (len-1))
			std::cout << " ";
	}

	if (len > 0)
		std::cout << std::endl;

	if (len > 0)
		return arg_list[0];
	else
		return 0;
}

static inline expr_t nchoosek_wrapper(expr_t * arg_list) {
unsigned long cnm = 1UL;
int i, f;
int n, m;

n = (int)arg_list[0];
m = (int)arg_list[1];

if (m*2 >n) m = n-m;
for (i=1 ; i <= m; n--, i++)
{
if ((f=n) % i == 0)
f   /= i;
else  cnm /= i;
cnm *= f;
}
return (expr_t)cnm;
}


static inline expr_t fact_wrapper(expr_t * arg_list) {


int result = 1;

int n = (int)arg_list[0];

while (n > 1) {
result = result * n;
n--;
}
return (expr_t)result;
}


static expr_t rgb_wrapper(expr_t * arg_list) {
	return packRGB(arg_list[0],arg_list[1],arg_list[2], 1.0f);
}

static expr_t rgba_wrapper(expr_t * arg_list) {
	return packRGB(arg_list[0],arg_list[1],arg_list[2],arg_list[3]);
}

};

#include <map>
class BuiltinFuncs {

public:
    
    static int init_builtin_func_db();
    static int destroy_builtin_func_db();
    static int load_all_builtin_func();
    static int load_builtin_func( const std::string & name, expr_t (*func_ptr)(expr_t*), int num_args );

    static int insert_func( Func *func );
    static Func *find_func( const std::string & name );
private:
     static std::map<std::string, Func*> builtin_func_tree;
     static volatile bool initialized;
};

#endif
