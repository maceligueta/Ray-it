#ifndef __Ray_it_constants__
#define __Ray_it_constants__

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

static unsigned int RAY_IT_ECHO_LEVEL = 1;
static std::string CURRENT_WORKING_DIR = "";

#ifdef RAY_IT_USE_FLOATS
//#pragma message("Compiling Ray-it with float (simple precision)")
typedef float real_number;
#define EPSILON 0.00001f
#define TOLERANCE_FOR_TESTS 0.00001f
#define ONE_THIRD 0.33333333333f
#define SQRT_OF_2_OVER_2 0.5f*M_SQRT2
#else
//#pragma message("Compiling Ray-it with double precision")
typedef double real_number;
#define EPSILON 0.0000000000001
#define TOLERANCE_FOR_TESTS 0.00001
#define ONE_THIRD 0.333333333333333333333
#define SQRT_OF_2_OVER_2 0.5*M_SQRT2
#endif

static const real_number SPEED_OF_LIGHT = 299792458;
static const real_number INVERSE_OF_SPEED_OF_LIGHT = 1.0 / SPEED_OF_LIGHT;
static const real_number IMPEDANCE_OF_FREE_SPACE = 376.730313668;
static const real_number INVERSE_OF_IMPEDANCE_OF_FREE_SPACE = 1.0 / IMPEDANCE_OF_FREE_SPACE;
static const real_number DEG_TO_RAD_FACTOR = M_PI / 180.0;
static const real_number RAD_TO_DEG_FACTOR = 180.0 * M_1_PI;

enum TypeOfResultsPrint {RESULTS_ON_NODES, RESULTS_ON_ELEMENTS};


#endif