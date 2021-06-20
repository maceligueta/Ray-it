#ifndef __BasicRaytracer_constants__
#define __BasicRaytracer_constants__

#pragma once

#include <math.h>

#ifdef RAY_IT_USE_DOUBLES
//#pragma message("Compiling Ray-it with double precision")
typedef double real;
#else
//#pragma message("Compiling Ray-it with float (simple precision)")
typedef float real;
#endif

#define EPSILON 0.00001f
#define ONE_THIRD 0.333333333333333f
enum TypeOfResultsPrint {RESULTS_ON_NODES, RESULTS_ON_ELEMENTS};


#endif