#ifndef __Ray_ittest16
#define __Ray_ittest16

#include "test.h"
#include "../src/computation.h"
#define _USE_MATH_DEFINES
#include <fstream>
#include <string>
#include <sstream>

class ComputationForTest16: public Computation {
    public:
    bool Run(const json& parameters) override {
        Computation::Run(parameters);

        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[50]->mIntensity,  real_number(0.000311092))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[100]->mIntensity, real_number(0.000165445))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[150]->mIntensity, real_number(0.000131197))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[200]->mIntensity, real_number(0.00011165))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[250]->mIntensity, real_number(9.4995e-05))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[300]->mIntensity, real_number(0.000439704))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[350]->mIntensity, real_number(0.000611452))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[400]->mIntensity, real_number(0.000312998))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[450]->mIntensity, real_number(0.000212023))) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[500]->mIntensity, real_number(9.79505e-05))) return 1;
        return 0;
    }
};

class Test16: public Test {

    public:

    Test16():Test(){
        mNumber = 16;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        json parameters;

        std::string parameters_filename  = "cases/bullington.json";
        InputsReader reader;
        RAY_IT_CURRENT_WORKING_DIR = reader.FindFolderOfFile(parameters_filename);
        if(reader.ReadInputParameters(parameters_filename, parameters)) return 1;

        ComputationForTest16 job;
        if( job.Run(parameters) ) return 1;

        return 0;
    }

};

#endif