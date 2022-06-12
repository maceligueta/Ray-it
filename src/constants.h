#ifndef __Ray_it_constants__
#define __Ray_it_constants__

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef RAY_IT_USE_FLOATS
//#pragma message("Compiling Ray-it with float (simple precision)")
typedef float real_number;
#define EPSILON 0.00001f
#define TOLERANCE_FOR_TESTS 0.00002f
#define ONE_THIRD 0.33333333333f
#else
//#pragma message("Compiling Ray-it with double precision")
typedef double real_number;
#define EPSILON 0.0000000000001
#define TOLERANCE_FOR_TESTS 0.00001
#define ONE_THIRD 0.333333333333333333333
#endif

#define SQRT_OF_2_OVER_2 real_number(0.5 * M_SQRT2)
static const real_number SPEED_OF_LIGHT = real_number(299792458);
static const real_number INVERSE_OF_SPEED_OF_LIGHT = real_number(1.0) / SPEED_OF_LIGHT;
static const real_number IMPEDANCE_OF_FREE_SPACE = real_number(376.730313668);
static const real_number INVERSE_OF_IMPEDANCE_OF_FREE_SPACE = real_number(1.0) / IMPEDANCE_OF_FREE_SPACE;
static const real_number DEG_TO_RAD_FACTOR = real_number(M_PI / 180.0);
static const real_number RAD_TO_DEG_FACTOR = real_number(180.0 * M_1_PI);
#define IMAGINARY_NUMBER std::complex<real_number>(0.0, static_cast<real_number>(1))

enum TypeOfResultsPrint {RESULTS_ON_NODES, RESULTS_ON_ELEMENTS};


#endif