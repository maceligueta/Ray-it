#ifndef __Ray_it_antenna__
#define __Ray_it_antenna__

#include "radiation_pattern.h"
#include "vector.h"

class Antenna {

    public:
    RadiationPattern mRadiationPattern;
    Vec3 mOrientation;


};

#endif