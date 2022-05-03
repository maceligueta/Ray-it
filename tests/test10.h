#ifndef __Ray_ittest10
#define __Ray_ittest10

#include "test.h"
#include "test7.h"
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
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        RadiationPattern pattern;
        Test7::FillRadiationPattern(pattern);
        pattern.mTotalPower = 1.0;

        AntennaVariables antenna_vars_1 = AntennaVariables();
        antenna_vars_1.mCoordinates = Vec3(0.0, 0.0, 0.0);
        antenna_vars_1.mName = "";
        antenna_vars_1.mVectorPointingFront = Vec3(1.0, 0.0, 0.0);
        antenna_vars_1.mVectorPointingUp = Vec3(0.0, 0.0, 1.0);
        antenna_vars_1.mRadiationPattern = pattern;

        AntennaVariables antenna_vars_2 = antenna_vars_1;
        antenna_vars_2.mVectorPointingFront = Vec3(0.0, 1.0, 0.0);
        antenna_vars_2.mVectorPointingUp = Vec3(-1.0, 0.0, 0.0);

        Antenna a1(antenna_vars_1);
        Antenna a2(antenna_vars_2);

        real_number p;
        p = a1.GetDirectionalPowerValue(Vec3(1.0, 0.0, 0.0));
        if(!CheckIfValuesAreEqual(p, 1.0)) return 1;

        p = a2.GetDirectionalPowerValue(Vec3(0.0, 1.0, 0.0));
        if(!CheckIfValuesAreEqual(p, 1.0)) return 1;

        p = a1.GetDirectionalPowerValue(Vec3(1.0, 1.0, 1.0));
        if(!CheckIfValuesAreEqual(p, real_number(0.26574300029755171))) return 1;

        p = a2.GetDirectionalPowerValue(Vec3(-1.0, 1.0, -1.0));
        if(!CheckIfValuesAreEqual(p, real_number(0.26574300029755171))) return 1;


        return 0;
    }

};

#endif