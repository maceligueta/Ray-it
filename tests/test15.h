#ifndef __Ray_ittest15
#define __Ray_ittest15

#include "test.h"
#include "../src/computation.h"
#define _USE_MATH_DEFINES
#include <fstream>
#include <string>
#include <sstream>

class ComputationForTest15: public Computation {
    public:
    bool Run(const json& parameters) override {
        Computation::Run(parameters);

        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[0]->mIntensity, 0.24699)) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[1]->mIntensity, 0.167739)) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[2]->mIntensity, 2.97226)) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[3]->mIntensity, 3.31896)) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[4]->mIntensity, 0.197424)) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[5]->mIntensity, 0.164267)) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[6]->mIntensity, 7.22847)) return 1;
        if(!Test::CheckIfValuesAreEqual(mMesh.mTriangles[7]->mIntensity, 4.89339)) return 1;
        return 0;
    }
};

class Test15: public Test {

    public:

    Test15():Test(){
        mNumber = 15;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        json parameters;

        std::string parameters_filename  = "cases/3faces.json";
        InputsReader reader;
        CURRENT_WORKING_DIR = reader.FindFolderOfFile(parameters_filename);
        if(reader.ReadInputParameters(parameters_filename, parameters)) return 1;

        ComputationForTest15 job;
        if( job.Run(parameters) ) return 1;

        return 0;
    }

};

#endif