#define _USE_MATH_DEFINES
#include <cmath>
#include "ray.h"
#include "mesh.h"
size_t Ray::mCounter = 0;

#define GLOBAL_PHOTON_COUNT 1000000

real_number randf(){
    return ((real_number)rand()/(real_number)RAND_MAX);
}

real_number sgn(real_number x){
    return (x >= 0)*real_number(2.0)-real_number(1.0);
}
Ray::Ray(const Vec3& start_position, const Vec3& direction):u(0),v(0) {
    mStartPosition = start_position;
    mDirection = Vec3::Normalize(direction);
    mInvDirection = Vec3(real_number(1.0)/direction.X(), real_number(1.0)/direction.Y(), real_number(1.0)/direction.Z());
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



