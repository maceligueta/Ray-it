#ifndef __Ray_ittest2
#define __Ray_ittest2

#include "test.h"

class Test2: public Test {

    public:

    Test2():Test(){
        mNumber = 2;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"...";
        RAY_IT_ECHO_LEVEL = 0;

        Mesh mesh;
        InputsReader reader;
        reader.ReadTerrainSTLMesh(mesh, "cases/square_test.stl");
        OutputsWriter writer;
        writer.PrintResultsInGidFormat(mesh, "cases/results2", TypeOfResultsPrint::RESULTS_ON_NODES);
        return CheckMeshResultsAreEqualToReference("cases/results2.post.msh", "cases/reference2.post.msh");

    }
};

#endif