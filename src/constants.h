#ifndef __BasicRaytracer_constants__
#define __BasicRaytracer_constants__

#include <math.h>

#ifdef RAY_IT_USE_DOUBLES
//#pragma message("Compiling Ray-it with double precision")
typedef double real_number;
#define EPSILON 0.000000001f
#else
//#pragma message("Compiling Ray-it with float (simple precision)")
typedef float real_number;
#define EPSILON 0.00001f
#endif


#define ONE_THIRD 0.333333333333333f
enum TypeOfResultsPrint {RESULTS_ON_NODES, RESULTS_ON_ELEMENTS};


#endif