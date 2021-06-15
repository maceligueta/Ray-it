#include "mesh.h"

//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
bool Mesh::IntersectBoundingBox(Ray &r) const {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    tmin = (mBoundingBox[r.mSign[0]].X() - r.mStartPosition.X()) * r.mInvDirection.X();
    tmax = (mBoundingBox[1-r.mSign[0]].X() - r.mStartPosition.X()) * r.mInvDirection.X();
    tymin = (mBoundingBox[r.mSign[1]].Y() - r.mStartPosition.Y()) * r.mInvDirection.Y();
    tymax = (mBoundingBox[1-r.mSign[1]].Y() - r.mStartPosition.Y()) * r.mInvDirection.Y();

    if ((tmin > tymax) || (tymin > tmax)) { return false; }
    if (tymin > tmin) { tmin = tymin; }
    if (tymax < tmax) { tmax = tymax; }

    tzmin = (mBoundingBox[r.mSign[2]].Z() - r.mStartPosition.Z()) * r.mInvDirection.Z();
    tzmax = (mBoundingBox[1-r.mSign[2]].Z() - r.mStartPosition.Z()) * r.mInvDirection.Z();

    if ((tmin > tzmax) || (tzmin > tmax)) { return false; }
    if (tzmin > tmin) { tmin = tzmin; }
    if (tzmax < tmax) { tmax = tzmax; }
    return  tmax > 0;
}

bool Mesh::Intersect(Ray &ray) const {
    if(!IntersectBoundingBox(ray)){ return false; }
    mTree.Traverse(ray);
    return ray.t_max < INFINITY;
}
