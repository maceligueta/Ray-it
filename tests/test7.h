#ifndef RayTracer_test7
#define RayTracer_test7

#include "test.h"
#include "raytracer.h"
#include "radiation_pattern.h"
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
        pattern.mSeparationBetweenPhiValues = 60.0f;
        pattern.mSeparationBetweenThetaValues = 60.0f;
        pattern.mRadiationMap.resize(360.0f / pattern.mSeparationBetweenPhiValues + 1);
        for(size_t i=0;i<pattern.mRadiationMap.size(); i++) {
            pattern.mRadiationMap[i].resize(180.0f / pattern.mSeparationBetweenThetaValues + 1);
        }

        pattern.mRadiationMap[0][0] = -50.0f; //phi = -180 theta = 0.0
        pattern.mRadiationMap[0][1] = -50.0f; //           theta = 60.0
        pattern.mRadiationMap[0][2] = -50.0f; //           theta = 120.0
        pattern.mRadiationMap[0][3] = -50.0f; //           theta = 180.0
        pattern.mRadiationMap[1][0] = -50.0f;  //phi = -120  theta = 0.0
        pattern.mRadiationMap[1][1] = -10.0f; //           theta = 60.0
        pattern.mRadiationMap[1][2] = -10.0f; //           theta = 120.0
        pattern.mRadiationMap[1][3] = -50.0f; //           theta = 180.0
        pattern.mRadiationMap[2][0] = -50.0f;  //phi = -60  theta = 0.0
        pattern.mRadiationMap[2][1] = -2.0f; //           theta = 60.0
        pattern.mRadiationMap[2][2] = -2.0f; //           theta = 120.0
        pattern.mRadiationMap[2][3] = -50.0f; //           theta = 180.0
        pattern.mRadiationMap[3][0] = -50.0f;  //phi = 0  theta = 0.0
        pattern.mRadiationMap[3][1] = 0.0f; //           theta = 60.0
        pattern.mRadiationMap[3][2] = 0.0f; //           theta = 120.0
        pattern.mRadiationMap[3][3] = -50.0f; //           theta = 180.0
        pattern.mRadiationMap[4][0] = -50.0f;  //phi = 60  theta = 0.0
        pattern.mRadiationMap[4][1] = -2.0f; //           theta = 60.0
        pattern.mRadiationMap[4][2] = -2.0f; //           theta = 120.0
        pattern.mRadiationMap[4][3] = -50.0f; //           theta = 180.0
        pattern.mRadiationMap[5][0] = -50.0f;  //phi = 120  theta = 0.0
        pattern.mRadiationMap[5][1] = -10.0f; //           theta = 60.0
        pattern.mRadiationMap[5][2] = -10.0f; //           theta = 60.0
        pattern.mRadiationMap[5][3] = -50.0f; //           theta = 180.0
        pattern.mRadiationMap[6][0] = -50.0f;  //phi = 180  theta = 0.0
        pattern.mRadiationMap[6][1] = -50.0f; //           theta = 60.0
        pattern.mRadiationMap[6][2] = -50.0f; //           theta = 120.0
        pattern.mRadiationMap[6][3] = -50.0f; //           theta = 180.0

        float p;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(0.0f, 90.0f));
        if(!CheckIfFloatsAreEqual(p, 0.0f)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(-110.0f, 90.0f));
        if(!CheckIfFloatsAreEqual(p, -8.66666698f)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(-24.0f, 90.0f));
        if(!CheckIfFloatsAreEqual(p, -0.8f)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(35.0f, 90.0f));
        if(!CheckIfFloatsAreEqual(p, -1.16666675f)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(89.0f, 90.0f));
        if(!CheckIfFloatsAreEqual(p, -5.86666679f)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(145.0f, 90.0f));
        if(!CheckIfFloatsAreEqual(p, -26.6666679f)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(0.0f, 45.0f));
        if(!CheckIfFloatsAreEqual(p, -12.5f)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(-40.0f, 45.0f));
        if(!CheckIfFloatsAreEqual(p, -13.5f)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(80.0f, 160.0f));
        if(!CheckIfFloatsAreEqual(p, -34.8888893f)) return 1;
        p = pattern.DirectionalPowerValue(SphericalCoordinates(-173.0f, 160.0f));
        if(!CheckIfFloatsAreEqual(p, -48.4444427f)) return 1;

        Mesh mesh;
        if(!ReadTerrainMesh(mesh, "cases/sphere.stl")) return 1;

        float base_power = 1.0f;
        for (size_t i=0; i<mesh.mNodes.size(); i++) {
            Vec3f dir(mesh.mNodes[i]);
            const float p_dB = pattern.DirectionalPowerValue(SphericalCoordinates(dir)); // in dB
            float power = base_power * std::pow(10.0f, p_dB*0.1f);
            mesh.mNodes[i] = dir * power;
        }

        PrintResultsInGidFormat(mesh, "cases/results7", TypeOfResultsPrint::RESULTS_ON_NODES);

        return !CheckMeshResultsAreEqualToReference("cases/results7.post.msh", "cases/reference7.post.msh");
    }

};

#endif