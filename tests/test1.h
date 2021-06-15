#ifndef RayTracer_test1
#define RayTracer_test1

#include "test.h"
#include "raytracer.h"
extern unsigned int echo_level;

class Test1: public Test {

    public:

    Test1():Test(){
        mNumber = 1;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"...";

        echo_level = 0;

        Mesh mesh;
        ReadTerrainMesh(mesh, "cases/square_test.stl");

        Vec3f origin(0.0f, 0.0f, 3.0f);
        Vec3f direction(0.1f, 0.0f, -1.0f);
        Ray test_ray1(origin, direction);
        if(!mesh.Intersect(test_ray1)) return 1;

        if(!CheckIfFloatsAreEqual(test_ray1.ComputeIntersectionPoint()[0], 0.3)) return 1;
        if(!CheckIfFloatsAreEqual(test_ray1.ComputeIntersectionPoint()[1], 0.0)) return 1;
        if(!CheckIfFloatsAreEqual(test_ray1.ComputeIntersectionPoint()[2], 0.0)) return 1;

        //This ray is parallel to the surface to be intersected:
        origin = Vec3f(0.0f, 0.0f, 0.0f);
        direction = Vec3f(1.0f, 1.0f, 0.0f);
        Ray test_ray2(origin, direction);
        if(mesh.Intersect(test_ray2)) return 1;
        //alternative approach
        Ray test_ray3(origin, direction);
        if(test_ray3.Intersect(mesh)) return 1;

        return 0;
    }
};

#endif