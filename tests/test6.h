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

        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3(1.0, 0.0, 0.0), SphericalCoordinates(0.0, 90.0), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3(1.0, 1.0, 1.0), SphericalCoordinates(45.0, real(54.735610)), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3(-1.0, 1.0, 1.0), SphericalCoordinates(135.0, real(54.735610)), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3(-1.0, -1.0, 1.0), SphericalCoordinates(-135.0, real(54.735610)), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3(-1.0, -1.0, -1.0), SphericalCoordinates(-135.0, real(125.2643896)), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3(0.0, 1.0, 0.0), SphericalCoordinates(90.0, 90.0), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3(0.0, 0.0, 1.0), SphericalCoordinates(0.0, 0.0), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3(0.0, 0.0, -1.0), SphericalCoordinates(0.0, 180.0), EPSILON)) return 1;
        if(!CheckDirectionToSphericalCoordinatesWithTolerance(Vec3(0.0, -1.0, 0.0), SphericalCoordinates(-90.0, 90.0), EPSILON)) return 1;

        return 0;

    }
    bool CheckDirectionToSphericalCoordinatesWithTolerance(const Vec3& dir, const SphericalCoordinates& expected_value, const real& tol){
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