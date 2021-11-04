#ifndef __Ray_ittest10
#define __Ray_ittest10

#include "test.h"
#include "../src/ray-it.h"
#include "../src/radiation_pattern.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>

class Test10: public Test {

    public:

    Test10():Test(){
        mNumber = 10;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"...";

        RAY_IT_ECHO_LEVEL = 0;

        RadiationPattern pattern;
        pattern.mTotalPower = 1.0;
        pattern.mSeparationBetweenPhiValues = 60.0;
        pattern.mSeparationBetweenThetaValues = 60.0;
        pattern.mRadiationMap.resize(int(360.0 / pattern.mSeparationBetweenPhiValues) + 1);
        for(size_t i=0;i<pattern.mRadiationMap.size(); i++) {
            pattern.mRadiationMap[i].resize(int(180.0 / pattern.mSeparationBetweenThetaValues) + 1);
            for (size_t j=0; j<pattern.mRadiationMap[i].size(); j++){
                pattern.mRadiationMap[i][j].resize(1);
            }
        }

        pattern.mRadiationMap[0][0][0] = -50.0; //phi = -180 theta = 0.0
        pattern.mRadiationMap[0][1][0] = -50.0; //           theta = 60.0
        pattern.mRadiationMap[0][2][0] = -50.0; //           theta = 120.0
        pattern.mRadiationMap[0][3][0] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[1][0][0] = -50.0;  //phi = -120  theta = 0.0
        pattern.mRadiationMap[1][1][0] = -10.0; //           theta = 60.0
        pattern.mRadiationMap[1][2][0] = -10.0; //           theta = 120.0
        pattern.mRadiationMap[1][3][0] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[2][0][0] = -50.0;  //phi = -60  theta = 0.0
        pattern.mRadiationMap[2][1][0] = -2.0; //           theta = 60.0
        pattern.mRadiationMap[2][2][0] = -2.0; //           theta = 120.0
        pattern.mRadiationMap[2][3][0] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[3][0][0] = -50.0;  //phi = 0  theta = 0.0
        pattern.mRadiationMap[3][1][0] = 0.0; //           theta = 60.0
        pattern.mRadiationMap[3][2][0] = 0.0; //           theta = 120.0
        pattern.mRadiationMap[3][3][0] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[4][0][0] = -50.0;  //phi = 60  theta = 0.0
        pattern.mRadiationMap[4][1][0] = -2.0; //           theta = 60.0
        pattern.mRadiationMap[4][2][0] = -2.0; //           theta = 120.0
        pattern.mRadiationMap[4][3][0] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[5][0][0] = -50.0;  //phi = 120  theta = 0.0
        pattern.mRadiationMap[5][1][0] = -10.0; //           theta = 60.0
        pattern.mRadiationMap[5][2][0] = -10.0; //           theta = 60.0
        pattern.mRadiationMap[5][3][0] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[6][0][0] = -50.0;  //phi = 180  theta = 0.0
        pattern.mRadiationMap[6][1][0] = -50.0; //           theta = 60.0
        pattern.mRadiationMap[6][2][0] = -50.0; //           theta = 120.0
        pattern.mRadiationMap[6][3][0] = -50.0; //           theta = 180.0

        Antenna a1, a2;
        a1.mRadiationPattern= pattern;
        a2.mRadiationPattern= pattern;

        a1.InitializeOrientation(Vec3(1.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0));
        a2.InitializeOrientation(Vec3(0.0, 1.0, 0.0), Vec3(-1.0, 0.0, 0.0));

        real_number p;
        p = a1.DirectionalPowerValue(Vec3(1.0, 0.0, 0.0));
        if(!CheckIfValuesAreEqual(p, 1.0)) return 1;

        p = a2.DirectionalPowerValue(Vec3(1.0, 0.0, 0.0));
        if(!CheckIfValuesAreEqual(p, 0.0)) return 1;


        return 0;
    }

};

#endif /* defined(__Ray_ittest10) */