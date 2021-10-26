#ifndef __Ray_it_triangle__
#define __Ray_it_triangle__

#include "vector.h"
#include "box.h"

class Mesh;

class Triangle {
public:
    unsigned int mId;
    Vec3 p0, p1, p2;
    Vec3 mFirstSide, mSecondSide, mNormal;
    real_number invDenom, uu, uv, vv;
    Box mBoundingBox;
    unsigned int mNodeIndices[3];
    Vec3 mCenter;
    real_number mIntensity = 0.0;

    Triangle(Mesh &mesh){}

    void SetEdgesAndPrecomputedValues(){
        mFirstSide = p1 - p0;
        mSecondSide = p2 - p0;
        mBoundingBox = ComputeBoundingBox();
        uu = Vec3::DotProduct(mFirstSide, mFirstSide);
        uv = Vec3::DotProduct(mFirstSide , mSecondSide);
        vv = Vec3::DotProduct(mSecondSide, mSecondSide);
        invDenom = real_number(1.0) / (uv*uv - uu*vv);
        mNormal = Vec3::normalize(Vec3::CrossProduct(mSecondSide, mFirstSide));
        mCenter = ComputeCenter();
    }

    Vec3 ComputeCenter() {
        return (p0 + p1 + p2) * ONE_THIRD;
    }

    bool Intersect(Ray &ray) const;

    real_number leftExtreme(int axis){
        return fmin(p0[axis], fmin(p1[axis], p2[axis]));
    }
    real_number rightExtreme(int axis){
        return fmax(p0[axis], fmax(p1[axis], p2[axis]));
    }

    Box ComputeBoundingBox(){
        real_number xmin = fmin(p0.X(), fmin(p1.X(), p2.X()));
        real_number ymin = fmin(p0.Y(), fmin(p1.Y(), p2.Y()));
        real_number zmin = fmin(p0.Z(), fmin(p1.Z(), p2.Z()));

        real_number xmax = fmax(p0.X(), fmax(p1.X(), p2.X()));
        real_number ymax = fmax(p0.Y(), fmax(p1.Y(), p2.Y()));
        real_number zmax = fmax(p0.Z(), fmax(p1.Z(), p2.Z()));


        return Box(Vec3(xmin, ymin, zmin), Vec3(xmax, ymax, zmax));
    }
    real_number ComputeArea();
};

#endif