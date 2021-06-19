#include "triangle.h"
#include "vector.h"
#include "constants.h"

float Triangle::ComputeArea(){
    return Vec3::cross(mFirstSide, mSecondSide).length() * 0.5f;
}

bool Triangle::Intersect(Ray &ray) const{
    // First check for plane intersection
    const Vec3 w0 = p0 - ray.mStartPosition;
    const float r1 = Vec3::DotProduct(mNormal, w0);
    const float r2 = Vec3::DotProduct(mNormal, ray.mDirection);

    // Check if the ray is parallel to the plane (within some epsilon)
    if(fabsf(r2) <= EPSILON) {
        return false;
    }
    const float r = r1 / r2;

    if( r < 0.00002f) {
        return false;
    }
    if (!ray.mTriangleFound || r < ray.t_max) {

        // Plane intersection confirmed. Check if we are inside the triangle:
        const Vec3 possible_intersection_point = ray.mStartPosition + ray.mDirection * r;
        const Vec3 w = possible_intersection_point - p0;
        const float uw = Vec3::DotProduct(mFirstSide, w);
        const float vw = Vec3::DotProduct(mSecondSide, w);
        const float s = (uv * vw - vv*uw) * invDenom;
        const float t = (uv * uw - uu*vw) * invDenom;
        const float tolerance = 1e-4f;
        if (s < 0.0-tolerance || t < 0.0-tolerance || s + t > 1.0f+tolerance){
            return false;
        } // Outside triangle

        // Triangle Intersection!
        ray.t_max= r;
        ray.u = s;
        ray.v = t;
        ray.mTriangleFound = true;
    }

    return ray.t_max < INFINITY;
}