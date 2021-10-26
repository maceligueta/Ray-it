#ifndef __BasicRaytracer_constants__
#define __BasicRaytracer_constants__

#include <math.h>

#ifdef RAY_IT_USE_FLOATS
//#pragma message("Compiling Ray-it with float (simple precision)")
typedef float real_number;
#define EPSILON 0.00001f
#define ONE_THIRD 0.33333333333f
#define SQRT_OF_2_OVER_2 0.5f*M_SQRT2
#else
//#pragma message("Compiling Ray-it with double precision")
typedef double real_number;
#define EPSILON 0.0000001
#define ONE_THIRD 0.333333333333333333333
#define SQRT_OF_2_OVER_2 0.5*M_SQRT2
#endif



enum TypeOfResultsPrint {RESULTS_ON_NODES, RESULTS_ON_ELEMENTS};


#endif