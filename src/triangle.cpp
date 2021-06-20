#include "triangle.h"
#include "vector.h"
#include "constants.h"

real Triangle::ComputeArea(){
    return Vec3::cross(mFirstSide, mSecondSide).length() * 0.5f;
}

bool Triangle::Intersect(Ray &ray) const{
    // First check for plane intersection
    const Vec3 w0 = p0 - ray.mStartPosition;
    const real r1 = Vec3::DotProduct(mNormal, w0);
    const real r2 = Vec3::DotProduct(mNormal, ray.mDirection);

    // Check if the ray is parallel to the plane (within some epsilon)
    if(std::abs(r2) <= EPSILON) {
        return false;
    }
    const real r = r1 / r2;

    if( r < 0.00002f) {
        return false;
    }
    if (!ray.mTriangleFound || r < ray.t_max) {

        // Plane intersection confirmed. Check if we are inside the triangle:
        const Vec3 possible_intersection_point = ray.mStartPosition + ray.mDirection * r;
        const Vec3 w = possible_intersection_point - p0;
        const real uw = Vec3::DotProduct(mFirstSide, w);
        const real vw = Vec3::DotProduct(mSecondSide, w);
        const real s = (uv * vw - vv*uw) * invDenom;
        const real t = (uv * uw - uu*vw) * invDenom;
        const real tolerance = 1e-4f;
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