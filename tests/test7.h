#ifndef RayTracer_test7
#define RayTracer_test7

#include "test.h"
#include "../src/ray-it.h"
#include "../src/radiation_pattern.h"
#define _USE_MATH_DEFINES
#include <math.h>

extern unsigned int echo_level;

class Test7: public Test {

    public:

    Test7():Test(){
        mNumber = 7;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"...";

        echo_level = 0;

        RadiationPattern pattern;
        pattern.mSeparationBetweenPhiValues = 60;
        pattern.mSeparationBetweenThetaValues = 60;
        pattern.mRadiationMap.resize(360 / pattern.mSeparationBetweenPhiValues + 1);
        for(size_t i=0;i<pattern.mRadiationMap.size(); i++) {
            pattern.mRadiationMap[i].resize(180 / pattern.mSeparationBetweenThetaValues + 1);
        }

        pattern.mRadiationMap[0][0] = -50.0; //phi = -180 theta = 0.0
        pattern.mRadiationMap[0][1] = -50.0; //           theta = 60.0
        pattern.mRadiationMap[0][2] = -50.0; //           theta = 120.0
        pattern.mRadiationMap[0][3] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[1][0] = -50.0;  //phi = -120  theta = 0.0
        pattern.mRadiationMap[1][1] = -10.0; //           theta = 60.0
        pattern.mRadiationMap[1][2] = -10.0; //           theta = 120.0
        pattern.mRadiationMap[1][3] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[2][0] = -50.0;  //phi = -60  theta = 0.0
        pattern.mRadiationMap[2][1] = -2.0; //           theta = 60.0
        pattern.mRadiationMap[2][2] = -2.0; //           theta = 120.0
        pattern.mRadiationMap[2][3] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[3][0] = -50.0;  //phi = 0  theta = 0.0
        pattern.mRadiationMap[3][1] = 0.0; //           theta = 60.0
        pattern.mRadiationMap[3][2] = 0.0; //           theta = 120.0
        pattern.mRadiationMap[3][3] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[4][0] = -50.0;  //phi = 60  theta = 0.0
        pattern.mRadiationMap[4][1] = -2.0; //           theta = 60.0
        pattern.mRadiationMap[4][2] = -2.0; //           theta = 120.0
        pattern.mRadiationMap[4][3] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[5][0] = -50.0;  //phi = 120  theta = 0.0
        pattern.mRadiationMap[5][1] = -10.0; //           theta = 60.0
        pattern.mRadiationMap[5][2] = -10.0; //           theta = 60.0
        pattern.mRadiationMap[5][3] = -50.0; //           theta = 180.0
        pattern.mRadiationMap[6][0] = -50.0;  //phi = 180  theta = 0.0
        pattern.mRadiationMap[6][1] = -50.0; //           theta = 60.0
        pattern.mRadiationMap[6][2] = -50.0; //           theta = 120.0
        pattern.mRadiationMap[6][3] = -50.0; //           theta = 180.0

        real p;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(0.0, 90.0));
        if(!CheckIfValuesAreEqual(p, 0.0)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(-110.0, 90.0));
        if(!CheckIfValuesAreEqual(p, real(-8.66666698))) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(-24.0, 90.0));
        if(!CheckIfValuesAreEqual(p, real(-0.8))) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(35.0, 90.0));
        if(!CheckIfValuesAreEqual(p, real(-1.16666675))) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(89.0, 90.0));
        if(!CheckIfValuesAreEqual(p, real(-5.86666679))) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(145.0, 90.0));
        if(!CheckIfValuesAreEqual(p, real(-26.6666679))) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(0.0, 45.0));
        if(!CheckIfValuesAreEqual(p, -12.5)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(-40.0, 45.0));
        if(!CheckIfValuesAreEqual(p, -13.5)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(80.0, 160.0));
        if(!CheckIfValuesAreEqual(p, real(-34.8888893))) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(-173.0, 160.0));
        if(!CheckIfValuesAreEqual(p, real(-48.4444427))) return 1;

        Mesh mesh;
        if(!ReadTerrainMesh(mesh, "cases/sphere.stl")) return 1;

        real base_power = 1.0;
        for (size_t i=0; i<mesh.mNodes.size(); i++) {
            Vec3 dir(mesh.mNodes[i]);
            const real p_dB = pattern.DirectionalPowerValue(SphericalCoordinates(dir)); // in dB
            real power = base_power * std::pow(real(10.0), p_dB*real(0.1));
            mesh.mNodes[i] = dir * power;
        }

        PrintResultsInGidFormat(mesh, "cases/results7", TypeOfResultsPrint::RESULTS_ON_NODES);

        return !CheckMeshResultsAreEqualToReference("cases/results7.post.msh", "cases/reference7.post.msh");
    }

};

#endif