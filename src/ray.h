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
	size_t mId;
    bool mTriangleFound = false;

    Vec3 mStartPosition;
    Vec3 mDirection;

    Vec3 mInvDirection;
    int mSign[3];

    real_number t_max;
    real_number u;
    real_number v;
    Vec3 intersectionNormal;
    real_number mPower = 0.0;

    Ray(const Vec3& mStartPosition, const Vec3& direction);

    Vec3 ComputeIntersectionPoint() const;
    bool Intersect(const Mesh& mesh);
};

#endif