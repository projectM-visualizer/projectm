/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id$
 *
 * $Log$
 */

/* Wrappers for all the builtin functions
   The arg_list pointer is a list of floats. Its
   size is equal to the number of arguments the parameter
   takes */

#ifndef _BUILTIN_FUNCS_H
#define _BUILTIN_FUNCS_H

#include "projectM.h"


/* Values to optimize the sigmoid function */
#define R  32767
#define RR 65534



inline float int_wrapper(float * arg_list) {

return floor(arg_list[0]);

}


inline float sqr_wrapper(float * arg_list) {

return pow(2, arg_list[0]);
}


inline float sign_wrapper(float * arg_list) {

return -arg_list[0];
}

inline float min_wrapper(float * arg_list) {

if (arg_list[0] > arg_list[1])
return arg_list[1];

return arg_list[0];
}

inline float max_wrapper(float * arg_list) {

if (arg_list[0] > arg_list[1])
return arg_list[0];

return arg_list[1];
}

/* consult your AI book */
inline float sigmoid_wrapper(float * arg_list) {
return (RR / (1 + exp( -(((float)(arg_list[0])) * arg_list[1]) / R) - R));
}


inline float bor_wrapper(float * arg_list) {

return (float)((int)arg_list[0] || (int)arg_list[1]);
}

inline float band_wrapper(float * arg_list) {
return (float)((int)arg_list[0] && (int)arg_list[1]);
}

inline float bnot_wrapper(float * arg_list) {
return (float)(!(int)arg_list[0]);
}

inline float if_wrapper(float * arg_list) {

if ((int)arg_list[0] == 0)
return arg_list[2];
return arg_list[1];
}


inline float rand_wrapper(float * arg_list) {
float l=1;

//  printf("RAND ARG:(%d)\n", (int)arg_list[0]);
if ((int)arg_list[0] > 0)
l = (float)((rand()) % ((int)arg_list[0]));
//printf("VAL: %f\n", l);
return l;
}

inline float equal_wrapper(float * arg_list) {
	return (arg_list[0] == arg_list[1]);
}


inline float above_wrapper(float * arg_list) {

return (arg_list[0] > arg_list[1]);
}


inline float below_wrapper(float * arg_list) {

return (arg_list[0] < arg_list[1]);
}

inline float sin_wrapper(float * arg_list) {
return (sin (arg_list[0]));
}


inline float cos_wrapper(float * arg_list) {
return (cos (arg_list[0]));
}

inline float tan_wrapper(float * arg_list) {
return (tan(arg_list[0]));
}

inline float asin_wrapper(float * arg_list) {
return (asin (arg_list[0]));
}

inline float acos_wrapper(float * arg_list) {
return (acos (arg_list[0]));
}

inline float atan_wrapper(float * arg_list) {
return (atan (arg_list[0]));
}

inline float atan2_wrapper(float * arg_list) {
return (atan2 (arg_list[0], arg_list[1]));
}

inline float pow_wrapper(float * arg_list) {
return (pow (arg_list[0], arg_list[1]));
}

inline float exp_wrapper(float * arg_list) {
return (exp(arg_list[0]));
}

inline float abs_wrapper(float * arg_list) {
return (fabs(arg_list[0]));
}

inline float log_wrapper(float* arg_list) {
return (log (arg_list[0]));
}

inline float log10_wrapper(float * arg_list) {
return (log10 (arg_list[0]));
}

inline float sqrt_wrapper(float * arg_list) {
return (sqrt (arg_list[0]));
}


inline float nchoosek_wrapper(float * arg_list) {
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
return (float)cnm;
}


inline float fact_wrapper(float * arg_list) {


int result = 1;

int n = (int)arg_list[0];

while (n > 1) {
result = result * n;
n--;
}
return (float)result;
}

#endif /** !_BUILTIN_FUNCS_H */
