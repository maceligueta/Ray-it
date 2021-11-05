#ifndef __Ray_ittest1
#define __Ray_ittest1

#include "test.h"
#include "../src/ray-it.h"

class Test1: public Test {

    public:

    Test1():Test(){
        mNumber = 1;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"... ";

        RAY_IT_ECHO_LEVEL = 0;

        Mesh mesh;
        InputsReader reader;
        reader.ReadTerrainSTLMesh(mesh, "cases/square_test.stl");

        KDTreeNode* root= new KDTreeNode();
        mesh.mTree = *root->RecursiveTreeNodeBuild(mesh.mTriangles, Box(mesh.mBoundingBox[0], mesh.mBoundingBox[1]), 0, SplitPlane());

        Vec3 origin(0.0, 0.0, 3.0);
        Vec3 direction(real_number(0.1), 0.0, -1.0);
        Ray test_ray1(origin, direction);
        if(!mesh.Intersect(test_ray1)) return 1;

        if(!CheckIfValuesAreEqual(test_ray1.ComputeIntersectionPoint()[0], real_number(0.3))) return 1;
        if(!CheckIfValuesAreEqual(test_ray1.ComputeIntersectionPoint()[1], 0.0)) return 1;
        if(!CheckIfValuesAreEqual(test_ray1.ComputeIntersectionPoint()[2], 0.0)) return 1;

        //This ray is parallel to the surface to be intersected:
        origin = Vec3(0.0, 0.0, 0.0);
        direction = Vec3(1.0, 1.0, 0.0);
        Ray test_ray2(origin, direction);
        if(mesh.Intersect(test_ray2)) return 1;
        //alternative approach
        Ray test_ray3(origin, direction);
        if(test_ray3.Intersect(mesh)) return 1;

        return 0;
    }
};

#endif