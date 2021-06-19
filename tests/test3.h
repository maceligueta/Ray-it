#ifndef RayTracer_test3
#define RayTracer_test3

#include "test.h"
#include "../src/ray-it.h"
extern unsigned int echo_level;

class Test3: public Test {

    public:

    Test3():Test(){
        mNumber = 3;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"...";

        echo_level = 0;

        Mesh mesh;
        ReadTerrainMesh(mesh, "cases/square_test.stl");

        Vec3 origin(0.0, 0.0, 3.0f);
        Vec3 direction(0.0, 0.0, 0.0);

        for(size_t i = 0; i<mesh.mNodes.size(); i++) {
            direction = Vec3(mesh.mNodes[i][0] - origin[0], mesh.mNodes[i][1] - origin[1], mesh.mNodes[i][2] - origin[2]);
            const float distance_squared = direction[0] * direction[0] + direction[1] *direction[1] + direction[2] * direction[2];
            mesh.mNodes[i].mIntensity = 1.0f / distance_squared;

            Ray test_ray(origin, direction);

            if(!test_ray.Intersect(mesh)){
                return 1;
            }
        }

        PrintResultsInGidFormat(mesh, "cases/results3", TypeOfResultsPrint::RESULTS_ON_NODES);
        return !CheckMeshResultsAreEqualToReference("cases/results3.post.res", "cases/reference3.post.res");
    }
};

#endif