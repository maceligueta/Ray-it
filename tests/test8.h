#ifndef RayTracer_test8
#define RayTracer_test8

#include "test.h"
#include "../src/ray-it.h"
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
        std::cout<<"Running test "<<mNumber<<"...";

        RAY_IT_ECHO_LEVEL = 0;

        RadiationPattern pattern;
        pattern.FillPatternInfoFrom4NEC2File("cases/Dipole1.out");

        real_number p;
        p = pattern.DirectionalGainValue(0.0, 90.0);
        if(!CheckIfValuesAreEqual(p, real_number(2.16))) return 1;
        p = pattern.DirectionalGainValue(-110.0, 90.0);
        if(!CheckIfValuesAreEqual(p, real_number(-9.12))) return 1;
        p = pattern.DirectionalGainValue(70.0, -90.0);
        if(!CheckIfValuesAreEqual(p, real_number(-9.12))) return 1;
        p = pattern.DirectionalGainValue(55.0, -70.0);
        if(!CheckIfValuesAreEqual(p, real_number(-3.04))) return 1;
        p = pattern.DirectionalRMSThetaPolarizationElectricFieldValue(55.0, -70.0);
        if(!CheckIfValuesAreEqual(p, real_number(SQRT_OF_2_OVER_2 * 1.53094E+01))) return 1;
        p = pattern.DirectionalRMSPhiPolarizationElectricFieldValue(55.0, -70.0);
        if(!CheckIfValuesAreEqual(p, real_number(SQRT_OF_2_OVER_2 * 3.13426E+01))) return 1;

        return 0;
    }

};

#endif