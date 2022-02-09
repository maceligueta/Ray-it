#ifndef __Ray_ittest12
#define __Ray_ittest12

#include "test7.h"
#include "../src/ray-it.h"
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
        std::cout<<"Running test "<<mNumber<<"... ";

        RAY_IT_ECHO_LEVEL = 1;

        RadiationPattern pattern;
        Test7::FillRadiationPattern(pattern);
        pattern.mTotalPower = 10.0;
        pattern.mMeasuringDistance = 1.0;

        real_number integral;
        integral = pattern.IntegratePatternSurfaceTotalPower();

        const real_number correction_factor = pattern.mTotalPower / integral;
        const real_number aux_log_correction_factor = 10.0 * log10(correction_factor);

        //scaling the pattern to make it consistent with the total power emitted
        for(size_t i=0; i<pattern.mRadiationMap.size(); i++){
            for(size_t j=0; j<pattern.mRadiationMap[i].size(); j++){
                //const real_number current_power_density = current_isotropic_power_density * std::pow(10.0, 0.1 * pattern.mRadiationMap[i][j].mGain);
                //const real_number corrected_power_density = correction_factor * current_power_density;
                //pattern.mRadiationMap[i][j].mGain = 10.0 * log10(corrected_power_density/current_isotropic_power_density);
                //The following line does the same job as the previous three!
                pattern.mRadiationMap[i][j].mGain += aux_log_correction_factor;
            }
        }

        //now checking that the integral is the total power emitter
        integral = pattern.IntegratePatternSurfaceTotalPower();
        if(!CheckIfValuesAreEqual(integral, pattern.mTotalPower)) return 1;

        //now overwriting all values to convert the antenna into an isotropic one (0.0 dB in all directions)
        for(size_t i=0; i<pattern.mRadiationMap.size(); i++){
            for(size_t j=0; j<pattern.mRadiationMap[i].size(); j++){
                pattern.mRadiationMap[i][j].mGain = 0.0;
            }
        }

        integral = pattern.IntegratePatternSurfaceTotalPower();
        if(!CheckIfValuesAreEqual(integral, pattern.mTotalPower)) return 1;

        return 0;
    }

};

#endif /* defined(__Ray_ittest12) */