#include "box.h"

//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
std::pair<real, real> Box::Intersect(Ray &r) const{
    real tmin, tmax, tymin, tymax, tzmin, tzmax;
    tmin = (GetBound(r.mSign[0]).X() - r.mStartPosition.X()) * r.mInvDirection.X();
    tmax = (GetBound(1-r.mSign[0]).X() - r.mStartPosition.X()) * r.mInvDirection.X();
    tymin = (GetBound(r.mSign[1]).Y() - r.mStartPosition.Y()) * r.mInvDirection.Y();
    tymax = (GetBound(1-r.mSign[1]).Y() - r.mStartPosition.Y()) * r.mInvDirection.Y();
    if ((tmin > tymax) || (tymin > tmax)){
        return std::pair<real,real>(real(0.0), real(0.0));
    }
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    tzmin = (GetBound(r.mSign[2]).Z() - r.mStartPosition.Z()) * r.mInvDirection.Z();
    tzmax = (GetBound(1-r.mSign[2]).Z() - r.mStartPosition.Z()) * r.mInvDirection.Z();
    if ((tmin > tzmax) || (tzmin > tmax)){
        return std::pair<real,real>(real(0.0), real(0.0));
    }
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    return  std::pair<real,real>(tmin, tmax);
}