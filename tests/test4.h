#ifndef __Ray_ittest4
#define __Ray_ittest4

#include "test.h"

class Test4: public Test {

    public:

    Test4():Test(){
        mNumber = 4;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        RAY_IT_ECHO_LEVEL = 0;

        Mesh mesh;
        InputsReader reader;
        reader.ReadTerrainSTLMesh(mesh, "cases/square_with_smaller_square_before_test.stl");

        KDTreeNode* root= new KDTreeNode();
        mesh.mTree = *root->RecursiveTreeNodeBuild(mesh.mTriangles, Box(mesh.mBoundingBox[0], mesh.mBoundingBox[1]), 0, SplitPlane());

        Vec3 origin(0.0, 0.0, 3.0);

        for(size_t i = 0; i<mesh.mNodes.size(); i++) {
            Vec3 vec_origin_to_node = Vec3(mesh.mNodes[i][0] - origin[0], mesh.mNodes[i][1] - origin[1], mesh.mNodes[i][2] - origin[2]);
            Ray test_ray(origin, vec_origin_to_node);
            test_ray.Intersect(mesh);
            const real_number distance_squared = vec_origin_to_node[0] * vec_origin_to_node[0] + vec_origin_to_node[1] *vec_origin_to_node[1] + vec_origin_to_node[2] * vec_origin_to_node[2];
            if(std::abs(test_ray.t_max * test_ray.t_max - distance_squared) < EPSILON) {
                mesh.mNodes[i].mIntensity = real_number(1.0) / distance_squared;
            }
        }

        OutputsWriter writer;
        std::vector<Antenna> antennas;
        writer.PrintResultsInGidFormat(mesh, antennas, "cases/results4", TypeOfResultsPrint::RESULTS_ON_NODES);
        #ifdef RAY_IT_USE_FLOATS
        std::string reference_result_file_name = "cases/reference4_float.post.res";
        #else
        std::string reference_result_file_name = "cases/reference4_double.post.res";
        #endif
        return CheckMeshResultsAreEqualToReference("cases/results4.post.res", reference_result_file_name);

    }
};

#endif