#define _USE_MATH_DEFINES
#include <cmath>
#include "ray.h"
#include "mesh.h"
size_t Ray::mCounter = 0;

#define GLOBAL_PHOTON_COUNT 1000000

float randf(){
    return ((float)rand()/(float)RAND_MAX);
}

float sgn(float x){
    return (x >= 0)*2.0f-1.0f;
}
Ray::Ray(const Vec3f& start_position, const Vec3f& direction):u(0),v(0) {
    mStartPosition = start_position;
    mDirection = Vec3f::normalize(direction);
    mInvDirection = Vec3f(1.0f/direction.X(), 1.0f/direction.Y(), 1.0f/direction.Z());
    mSign[0] = (mInvDirection.X() < 0);
    mSign[1] = (mInvDirection.Y() < 0);
    mSign[2] = (mInvDirection.Z() < 0);
    t_max = INFINITY;
}

Vec3f Ray::ComputeIntersectionPoint() const {
    return mStartPosition + mDirection * t_max;
}

bool Ray::Intersect(const Mesh& mesh) {
    return mesh.Intersect(*this);
}



