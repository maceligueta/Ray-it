#ifndef __Ray_it_antenna__
#define __Ray_it_antenna__

#include "constants.h"
#include "radiation_pattern.h"
#include "vector.h"

class Antenna {

    public:
    std::string mName;
    Vec3 mCoordinates;
    RadiationPattern mRadiationPattern;
    Vec3 mOrientation;


};

#endif