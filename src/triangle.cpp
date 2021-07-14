#include "triangle.h"
#include "vector.h"
#include "constants.h"

real_number Triangle::ComputeArea(){
    return Vec3::CrossProduct(mFirstSide, mSecondSide).length() * real_number(0.5);
}

bool Triangle::Intersect(Ray &ray) const{

    Vec3 pvec = Vec3::CrossProduct(ray.mDirection, mSecondSide);
    const real_number det = Vec3::DotProduct(mFirstSide, pvec);

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < EPSILON) return false;

    const real_number invDet = 1.0 / det;

    Vec3 tvec = ray.mStartPosition - p0;
    const real_number u = Vec3::DotProduct(tvec, pvec) * invDet;
    const real_number tolerance = EPSILON;
    if (u < 0 - tolerance || u > 1.0 + tolerance) return false;

    Vec3 qvec = Vec3::CrossProduct(tvec, mFirstSide);
    const real_number v = Vec3::DotProduct(ray.mDirection, qvec) * invDet;
    if (v < 0 - tolerance || u + v > 1.0 + tolerance) return false;

    const real_number t = Vec3::DotProduct(mSecondSide, qvec) * invDet;

    if(!ray.mTriangleFound || t < ray.t_max) {
        ray.t_max= t;
        ray.u = u;
        ray.v = v;
        ray.mTriangleFound = true;
    }

    return ray.t_max < INFINITY;

/*
    // First check for plane intersection
    const Vec3 w0 = p0 - ray.mStartPosition;
    const real_number r1 = Vec3::DotProduct(mNormal, w0);
    const real_number r2 = Vec3::DotProduct(mNormal, ray.mDirection);

    // Check if the ray is parallel to the plane (within some epsilon)
    if(std::abs(r2) <= EPSILON) {
        return false;
    }
    const real_number r = r1 / r2;

    if( r < 0.00002f) {
        return false;
    }
    if (!ray.mTriangleFound || r < ray.t_max) {

        // Plane intersection confirmed. Check if we are inside the triangle:
        const Vec3 possible_intersection_point = ray.mStartPosition + ray.mDirection * r;
        const Vec3 w = possible_intersection_point - p0;
        const real_number uw = Vec3::DotProduct(mFirstSide, w);
        const real_number vw = Vec3::DotProduct(mSecondSide, w);
        const real_number s = (uv * vw - vv*uw) * invDenom;
        const real_number t = (uv * uw - uu*vw) * invDenom;
        const real_number tolerance = EPSILON;
        if (s < 0.0-tolerance || t < 0.0-tolerance || s + t > real_number(1.0)+tolerance){
            return false;
        } // Outside triangle

        // Triangle Intersection!
        ray.t_max= r;
        ray.u = s;
        ray.v = t;
        ray.mTriangleFound = true;
    }

    return ray.t_max < INFINITY;
    */
}