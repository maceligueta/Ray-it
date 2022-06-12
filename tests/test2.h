#ifndef __Ray_ittest2
#define __Ray_ittest2

#include "test.h"
#include "../src/outputs_writer.h"

class Test2: public Test {

    public:

    Test2():Test(){
        mNumber = 2;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";
        Mesh mesh;
        InputsReader reader;
        reader.ReadTerrainSTLMesh(mesh, "cases/square_test.stl");
        OutputsWriter writer;
        std::vector<Antenna> antennas;
        writer.PrintResultsInGidFormat(mesh, antennas, "cases/results2", TypeOfResultsPrint::RESULTS_ON_NODES);
        return CheckMeshResultsAreEqualToReference("cases/results2.post.msh", "cases/reference2.post.msh");

    }
};

#endif