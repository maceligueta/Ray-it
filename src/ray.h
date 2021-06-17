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

    Vec3f mStartPosition;
    Vec3f mDirection;

    Vec3f mInvDirection;
    int mSign[3];

    float t_max;
    float u;
    float v;
    Vec3f intersectionNormal;
    float mPower = 0.0f;

    Ray(const Vec3f& mStartPosition, const Vec3f& direction);

    Vec3f ComputeIntersectionPoint() const;
    bool Intersect(const Mesh& mesh);
};

#endif