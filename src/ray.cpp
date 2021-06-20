#define _USE_MATH_DEFINES
#include <cmath>
#include "ray.h"
#include "mesh.h"
size_t Ray::mCounter = 0;

#define GLOBAL_PHOTON_COUNT 1000000

real randf(){
    return ((real)rand()/(real)RAND_MAX);
}

real sgn(real x){
    return (x >= 0)*2.0f-1.0f;
}
Ray::Ray(const Vec3& start_position, const Vec3& direction):u(0),v(0) {
    mStartPosition = start_position;
    mDirection = Vec3::normalize(direction);
    mInvDirection = Vec3(1.0f/direction.X(), 1.0f/direction.Y(), 1.0f/direction.Z());
    mSign[0] = (mInvDirection.X() < 0);
    mSign[1] = (mInvDirection.Y() < 0);
    mSign[2] = (mInvDirection.Z() < 0);
    t_max = INFINITY;
}

Vec3 Ray::ComputeIntersectionPoint() const {
    return mStartPosition + mDirection * t_max;
}

bool Ray::Intersect(const Mesh& mesh) {
    return mesh.Intersect(*this);
}



