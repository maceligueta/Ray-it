#ifndef RayTracer_test6
#define RayTracer_test6

#include "test.h"
#include "../src/ray-it.h"
#include "../src/radiation_pattern.h"
#define _USE_MATH_DEFINES
#include <math.h>

extern unsigned int echo_level;

class Test6: public Test {

    public:

    Test6():Test(){
        mNumber = 6;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"...";

        echo_level = 0;

        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3f(1.0f, 0.0f, 0.0f), SphericalCoordinates(0.0f, 90.0f), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3f(1.0f, 1.0f, 1.0f), SphericalCoordinates(45.0f, 54.735610f), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3f(-1.0f, 1.0f, 1.0f), SphericalCoordinates(135.0f, 54.735610f), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3f(-1.0f, -1.0f, 1.0f), SphericalCoordinates(-135.0f, 54.735610f), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3f(-1.0f, -1.0f, -1.0f), SphericalCoordinates(-135.0f, 125.2643896f), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3f(0.0f, 1.0f, 0.0f), SphericalCoordinates(90.0f, 90.0f), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3f(0.0f, 0.0f, 1.0f), SphericalCoordinates(0.0f, 0.0f), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3f(0.0f, 0.0f, -1.0f), SphericalCoordinates(0.0f, 180.0f), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3f(0.0f, -1.0f, 0.0f), SphericalCoordinates(-90.0f, 90.0f), EPSILON)) return 1;

        return 0;

    }
    bool CheckDirectionToSphericalCoordinatesWithTolerance(const Vec3f& dir, const SphericalCoordinates& expected_value, const float& tol){
        SphericalCoordinates spherical_coords(dir);
        if ( std::abs(spherical_coords.mPhi-expected_value.mPhi) > tol) {
            return false;
        }
        if ( std::abs(spherical_coords.mTheta-expected_value.mTheta) > tol) {
            return false;
        }
        return true;
    }
};

#endif