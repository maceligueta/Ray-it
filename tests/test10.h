#ifndef __Ray_ittest10
#define __Ray_ittest10

#include "test.h"
#include "test7.h"
#include "../src/ray-it.h"
#include "../src/radiation_pattern.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>

class Test10: public Test7 {

    public:

    Test10():Test7(){
        mNumber = 10;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"... ";

        RAY_IT_ECHO_LEVEL = 0;

        RadiationPattern pattern;
        Test7::FillRadiationPattern(pattern);
        pattern.mTotalPower = 1.0;

        Antenna a1, a2;
        a1.mRadiationPattern= pattern;
        a2.mRadiationPattern= pattern;

        a1.InitializeOrientation(Vec3(1.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0));
        a2.InitializeOrientation(Vec3(0.0, 1.0, 0.0), Vec3(-1.0, 0.0, 0.0));

        real_number p;
        p = a1.GetDirectionalPowerValue(Vec3(1.0, 0.0, 0.0));
        if(!CheckIfValuesAreEqual(p, 1.0)) return 1;

        p = a2.GetDirectionalPowerValue(Vec3(0.0, 1.0, 0.0));
        if(!CheckIfValuesAreEqual(p, 1.0)) return 1;

        p = a1.GetDirectionalPowerValue(Vec3(1.0, 1.0, 1.0));
        if(!CheckIfValuesAreEqual(p, 0.26574300029755171)) return 1;

        p = a2.GetDirectionalPowerValue(Vec3(-1.0, 1.0, -1.0));
        if(!CheckIfValuesAreEqual(p, 0.26574300029755171)) return 1;


        return 0;
    }

};

#endif /* defined(__Ray_ittest10) */