#ifndef __Ray_ittest12
#define __Ray_ittest12

#include "test7.h"
#include "../src/radiation_pattern.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>

class Test12: public Test7 {

    public:

    Test12():Test7(){
        mNumber = 12;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        RAY_IT_ECHO_LEVEL = 0;

        RadiationPattern pattern;
        Test7::FillRadiationPattern(pattern);
        pattern.mTotalPower = 10.0;
        pattern.mMeasuringDistance = 1.0;

        pattern.ScaleDirectionalGainForConsistentIntegral();

        //now checking that the integral is the total power emitter
        real_number integral = pattern.IntegratePatternSurfaceTotalPowerBasedOnGain();
        if(!CheckIfValuesAreEqual(integral, pattern.mTotalPower)) return 1;

        //now overwriting all values to convert the antenna into an isotropic one (0.0 dB in all directions)
        for(size_t i=0; i<pattern.mRadiationMap.size(); i++){
            for(size_t j=0; j<pattern.mRadiationMap[i].size(); j++){
                pattern.mRadiationMap[i][j][Gain] = 0.0;
            }
        }

        integral = pattern.IntegratePatternSurfaceTotalPowerBasedOnGain();
        if(!CheckIfValuesAreEqual(integral, pattern.mTotalPower)) return 1;

        return 0;
    }

};

#endif