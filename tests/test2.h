#ifndef RayTracer_test2
#define RayTracer_test2

#include "test.h"

class Test2: public Test {

    public:

    Test2():Test(){
        mNumber = 2;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"...";        
        echo_level = 0;

        Mesh mesh; 
        ReadTerrainMesh(mesh, "cases/square_test.stl");
        PrintResultsInGidFormat(mesh, "cases/results2", TypeOfResultsPrint::RESULTS_ON_NODES);
        return CheckMeshResultsAreEqualToReference("cases/results2.post.msh", "cases/reference2.post.msh");

    }    
};

#endif