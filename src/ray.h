#ifndef __RAY_H
#define __RAY_H
#include <vector>
#include <math.h>
#include <unordered_set>
#include "vector.h"

class Mesh;

class Ray {

public:
	static size_t mCounter;
    Vec3 mStartPosition;
    Vec3 mDirection;
    Vec3 mInvDirection;
    int mSign[3];
    real_number t_max;
    real_number u;
    real_number v;
    int mIdOfFirstCrossedTriangle = -1;


    Ray(const Vec3& mStartPosition, const Vec3& direction);

    Vec3 ComputeIntersectionPoint() const;
    bool Intersect(const Mesh& mesh);
    inline bool ConfirmHitAtDistance(const real_number& distance, const real_number& tolerance) {
        return std::abs(t_max - distance) < tolerance;
    }
};

#endif