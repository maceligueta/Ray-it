#ifndef __Ray_ittest7
#define __Ray_ittest7

#include "test.h"
#include "../src/ray-it.h"
#include "../src/radiation_pattern.h"
#define _USE_MATH_DEFINES
#include <math.h>

class Test7: public Test {

    public:

    Test7():Test(){
        mNumber = 7;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"...";

        RAY_IT_ECHO_LEVEL = 0;

        RadiationPattern pattern;
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

        real_number p;
        p = pattern.DirectionalGainValue(0.0, 90.0);
        if(!CheckIfValuesAreEqual(p, 0.0)) return 1;
        p = pattern.DirectionalGainValue(-110.0, 90.0);
        if(!CheckIfValuesAreEqual(p, real_number(-8.6666666666666661))) return 1;
        p = pattern.DirectionalGainValue(-24.0, 90.0);
        if(!CheckIfValuesAreEqual(p, real_number(-0.8))) return 1;
        p = pattern.DirectionalGainValue(35.0, 90.0);
        if(!CheckIfValuesAreEqual(p, real_number(-1.16666675))) return 1;
        p = pattern.DirectionalGainValue(89.0, 90.0);
        if(!CheckIfValuesAreEqual(p, real_number(-5.8666666666666671))) return 1;
        p = pattern.DirectionalGainValue(145.0, 90.0);
        if(!CheckIfValuesAreEqual(p, real_number(-26.666666666666664))) return 1;
        p = pattern.DirectionalGainValue(0.0, 45.0);
        if(!CheckIfValuesAreEqual(p, -12.5)) return 1;
        p = pattern.DirectionalGainValue(-40.0, 45.0);
        if(!CheckIfValuesAreEqual(p, -13.5)) return 1;
        p = pattern.DirectionalGainValue(80.0, 160.0);
        if(!CheckIfValuesAreEqual(p, real_number(-34.888888888888893))) return 1;
        p = pattern.DirectionalGainValue(-173.0, 160.0);
        if(!CheckIfValuesAreEqual(p, real_number(-48.444444444444443))) return 1;

        Mesh mesh;
        InputsReader reader;
        if(reader.ReadTerrainSTLMesh(mesh, "cases/sphere.stl")) return 1;

        real_number base_power = 1.0;
        for (size_t i=0; i<mesh.mNodes.size(); i++) {
            Vec3 dir(mesh.mNodes[i]);
            const real_number p_dB = pattern.DirectionalGainValue(SphericalCoordinates(dir)); // in dB
            real_number power = base_power * std::pow(real_number(10.0), p_dB*real_number(0.1));
            mesh.mNodes[i] = dir * power;
        }

        OutputsWriter writer;
        std::vector<Antenna> antennas;
        writer.PrintResultsInGidFormat(mesh, antennas, "cases/results7", TypeOfResultsPrint::RESULTS_ON_NODES);

        #ifdef RAY_IT_USE_FLOATS
        std::string reference_result_file_name = "cases/reference7_float.post.msh";
        #else
        std::string reference_result_file_name = "cases/reference7_double.post.msh";
        #endif

        return CheckMeshResultsAreEqualToReference("cases/results7.post.msh", reference_result_file_name);
    }

};

#endif