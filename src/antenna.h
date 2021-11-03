#ifndef __Ray_it_antenna__
#define __Ray_it_antenna__

#include "constants.h"
#include "radiation_pattern.h"
#include "vector.h"
#include <math.h>

class Antenna {

    public:
    std::string mName;
    Vec3 mCoordinates;
    RadiationPattern mRadiationPattern;
    Vec3 mOrientation;

    real_number DirectionalPowerValue(const Vec3& cartesian_direction) const {
        real_number power_out = mRadiationPattern.mTotalPower * std::pow(10.0, 0.1* mRadiationPattern.DirectionalGainValue(cartesian_direction));
        return power_out;
    }

    bool LoadRadiationPattern(const std::string& radiation_pattern_file_name) {
        mRadiationPattern.FillPatternInfoFrom4NEC2File(radiation_pattern_file_name);
        return 0;
    }

};

#endif