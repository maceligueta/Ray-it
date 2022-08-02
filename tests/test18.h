#ifndef __Ray_ittest18
#define __Ray_ittest18

#include "test.h"
#include "../src/computation.h"
#define _USE_MATH_DEFINES
#include <fstream>
#include <string>
#include <sstream>

class ComputationForTest18: public Computation {
    public:
    bool Run(const json& parameters) override {
        Computation::Run(parameters);

        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[0]->mIntensity,  real_number(0.00155082))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[100]->mIntensity, real_number(0.0))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[150]->mIntensity, real_number(0.000618464))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[200]->mIntensity, real_number(0.00135554))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[250]->mIntensity, real_number(0.000175592))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[300]->mIntensity, real_number(0.00429186))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[350]->mIntensity, real_number(0.0))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[400]->mIntensity, real_number(0.164134))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[450]->mIntensity, real_number(0.00520601))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[500]->mIntensity, real_number(0.0))) return 1;

        return 0;
    }
};

class Test18: public Test {

    public:

    Test18():Test(){
        mNumber = 18;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        json parameters;

        std::string parameters_filename  = "cases/dxf_buildings_no_terrain.json";
        InputsReader reader;
        RAY_IT_CURRENT_WORKING_DIR = reader.FindFolderOfFile(parameters_filename);
        if(reader.ReadInputParameters(parameters_filename, parameters)) return 1;

        ComputationForTest18 job;
        if( job.Run(parameters) ) return 1;

        return 0;
    }

};

#endif