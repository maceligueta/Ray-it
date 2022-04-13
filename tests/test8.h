#ifndef RayTracer_test8
#define RayTracer_test8

#include "test.h"
#include "../src/radiation_pattern.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>

class Test8: public Test {

    public:

    Test8():Test(){
        mNumber = 8;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        RAY_IT_ECHO_LEVEL = 0;

        RadiationPattern pattern;
        pattern.FillPatternInfoFrom4NEC2File("cases/Dipole1.out");

        real_number p;
        p = pattern.GetDirectionalGainValue(0.0, 90.0);
        if(!CheckIfValuesAreEqual(p, real_number(2.16))) return 1;
        p = pattern.GetDirectionalGainValue(-110.0, 90.0);
        if(!CheckIfValuesAreEqual(p, real_number(-9.12))) return 1;
        p = pattern.GetDirectionalGainValue(70.0, -90.0);
        if(!CheckIfValuesAreEqual(p, real_number(-9.12))) return 1;
        p = pattern.GetDirectionalGainValue(55.0, -70.0);
        if(!CheckIfValuesAreEqual(p, real_number(-3.04))) return 1;
        p = pattern.GetDirectionalRMSThetaPolarizationElectricFieldValue(55.0, -70.0);
        if(!CheckIfValuesAreEqual(p, real_number(SQRT_OF_2_OVER_2 * 1.53094E+01))) return 1;
        p = pattern.GetDirectionalRMSPhiPolarizationElectricFieldValue(55.0, -70.0);
        if(!CheckIfValuesAreEqual(p, real_number(SQRT_OF_2_OVER_2 * 3.13426E+01))) return 1;

        return 0;
    }

};

#endif