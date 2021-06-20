#ifndef RayTracer_test4
#define RayTracer_test4

#include "test.h"
#include "../src/ray-it.h"
extern unsigned int echo_level;

class Test4: public Test {

    public:

    Test4():Test(){
        mNumber = 4;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"...";

        echo_level = 0;

        Mesh mesh;
        ReadTerrainMesh(mesh, "cases/square_with_smaller_square_before_test.stl");

        Vec3 origin(0.0, 0.0, 3.0f);

        for(size_t i = 0; i<mesh.mNodes.size(); i++) {
            Vec3 vec_origin_to_node = Vec3(mesh.mNodes[i][0] - origin[0], mesh.mNodes[i][1] - origin[1], mesh.mNodes[i][2] - origin[2]);
            Ray test_ray(origin, vec_origin_to_node);
            test_ray.Intersect(mesh);
            const real distance_squared = vec_origin_to_node[0] * vec_origin_to_node[0] + vec_origin_to_node[1] *vec_origin_to_node[1] + vec_origin_to_node[2] * vec_origin_to_node[2];
            if(std::abs(test_ray.t_max * test_ray.t_max - distance_squared) < 1e-4f) {
                mesh.mNodes[i].mIntensity = 1.0f / distance_squared;
            }
        }

        PrintResultsInGidFormat(mesh, "cases/results4", TypeOfResultsPrint::RESULTS_ON_NODES);
        return !CheckMeshResultsAreEqualToReference("cases/results4.post.res", "cases/reference4.post.res");

    }
};

#endif