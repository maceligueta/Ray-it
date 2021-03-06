#ifndef __Ray_ittest3
#define __Ray_ittest3

#include "test.h"

class Test3: public Test {

    public:

    Test3():Test(){
        mNumber = 3;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        Mesh mesh;
        InputsReader reader;
        reader.ReadTerrainSTLMesh(mesh, "cases/square_test.stl");

        KDTreeNode* root= new KDTreeNode();
        mesh.mTree = *root->RecursiveTreeNodeBuild(mesh.mTriangles, Box(mesh.mBoundingBox[0], mesh.mBoundingBox[1]), 0, SplitPlane());

        Vec3 origin(0.0, 0.0, 3.0);
        Vec3 direction(0.0, 0.0, 0.0);

        for(size_t i = 0; i<mesh.mNodes.size(); i++) {
            direction = Vec3(mesh.mNodes[i][0] - origin[0], mesh.mNodes[i][1] - origin[1], mesh.mNodes[i][2] - origin[2]);
            const real_number distance_squared = direction[0] * direction[0] + direction[1] *direction[1] + direction[2] * direction[2];
            mesh.mNodes[i].mIntensity = real_number(1.0) / distance_squared;

            Ray test_ray(origin, direction);

            if(!test_ray.Intersect(mesh)){
                return 1;
            }
        }

        OutputsWriter writer;
        std::vector<Antenna> antennas;
        writer.PrintResultsInGidFormat(mesh, antennas, "cases/results3", TypeOfResultsPrint::RESULTS_ON_NODES);
        #ifdef RAY_IT_USE_FLOATS
        std::string reference_result_file_name = "cases/reference3_float.post.res";
        #else
        std::string reference_result_file_name = "cases/reference3_double.post.res";
        #endif
        return CheckMeshResultsAreEqualToReference("cases/results3.post.res", reference_result_file_name);
    }
};

#endif