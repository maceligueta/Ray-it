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
    float invDenom, uu, uv, vv;
    Box mBoundingBox;
    unsigned int mNodeIndices[3];
    Vec3 mCenter;
    float mIntensity;

    Triangle(Mesh &mesh){}

    void SetEdgesAndPrecomputedValues(){
        mFirstSide = p1 - p0;
        mSecondSide = p2 - p0;
        mBoundingBox = ComputeBoundingBox();
        uu = Vec3::DotProduct(mFirstSide, mFirstSide);
        uv = Vec3::DotProduct(mFirstSide , mSecondSide);
        vv = Vec3::DotProduct(mSecondSide, mSecondSide);
        invDenom = 1.0f / (uv*uv - uu*vv);
        mNormal = Vec3::cross(mSecondSide, mFirstSide);
        mCenter = ComputeCenter();
    }

    Vec3 ComputeCenter() {
        return (p0 + p1 + p2) * (1.0f/3.0f);
    }

    bool Intersect(Ray &ray) const;

    float leftExtreme(int axis){
        return fmin(p0[axis], fmin(p1[axis], p2[axis]));
    }
    float rightExtreme(int axis){
        return fmax(p0[axis], fmax(p1[axis], p2[axis]));
    }

    Box ComputeBoundingBox(){
        float xmin = fmin(p0.X(), fmin(p1.X(), p2.X()));
        float ymin = fmin(p0.Y(), fmin(p1.Y(), p2.Y()));
        float zmin = fmin(p0.Z(), fmin(p1.Z(), p2.Z()));

        float xmax = fmax(p0.X(), fmax(p1.X(), p2.X()));
        float ymax = fmax(p0.Y(), fmax(p1.Y(), p2.Y()));
        float zmax = fmax(p0.Z(), fmax(p1.Z(), p2.Z()));


        return Box(Vec3(xmin, ymin, zmin), Vec3(xmax, ymax, zmax));
    }
    float ComputeArea();
};

#endif