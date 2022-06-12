#ifndef __Ray_it_triangle__
#define __Ray_it_triangle__

#include "vector.h"
#include "box.h"
#include "jones.h"
#include "node.h"

class Mesh;

class Triangle {
public:
    unsigned int mNodeIndices[3];
    Vec3 mP0, mP1, mP2;
    Vec3 mFirstSide, mSecondSide;
    Vec3 mLocalAxis1, mLocalAxis2, mNormal;
    real_number invDenom, uu, uv, vv;
    Box mBoundingBox;
    Vec3 mCenter;
    real_number mIntensity = 0.0;
    VecC3 mElectricField;

    Triangle(const Node& n0, const Node& n1, const Node& n2){
        mNodeIndices[0] = n0.mId;
        mNodeIndices[1] = n1.mId;
        mNodeIndices[2] = n2.mId;
        mP0 = n0;
        mP1 = n1;
        mP2 = n2;
        mFirstSide = mP1 - mP0;
        mSecondSide = mP2 - mP0;
        mBoundingBox = ComputeBoundingBox();
        uu = Vec3::DotProduct(mFirstSide, mFirstSide);
        uv = Vec3::DotProduct(mFirstSide , mSecondSide);
        vv = Vec3::DotProduct(mSecondSide, mSecondSide);
        invDenom = real_number(1.0) / (uv*uv - uu*vv);
        mNormal = Vec3::Normalize(Vec3::CrossProduct(mFirstSide, mSecondSide));
        mLocalAxis1 = Vec3::Normalize(mFirstSide);
        mLocalAxis2 = Vec3::Normalize(Vec3::CrossProduct(mNormal, mFirstSide));
        mCenter = ComputeCenter();
        mElectricField = VecC3(0.0, 0.0, 0.0);
    }

    Vec3 ComputeCenter() {
        return (mP0 + mP1 + mP2) * ONE_THIRD;
    }

    bool Intersect(Ray &ray) const;

    real_number leftExtreme(int axis){
        return fmin(mP0[axis], fmin(mP1[axis], mP2[axis]));
    }
    real_number rightExtreme(int axis){
        return fmax(mP0[axis], fmax(mP1[axis], mP2[axis]));
    }

    Box ComputeBoundingBox(){
        real_number xmin = fmin(mP0.X(), fmin(mP1.X(), mP2.X()));
        real_number ymin = fmin(mP0.Y(), fmin(mP1.Y(), mP2.Y()));
        real_number zmin = fmin(mP0.Z(), fmin(mP1.Z(), mP2.Z()));

        real_number xmax = fmax(mP0.X(), fmax(mP1.X(), mP2.X()));
        real_number ymax = fmax(mP0.Y(), fmax(mP1.Y(), mP2.Y()));
        real_number zmax = fmax(mP0.Z(), fmax(mP1.Z(), mP2.Z()));


        return Box(Vec3(xmin, ymin, zmin), Vec3(xmax, ymax, zmax));
    }
    real_number ComputeArea() const;

    void ProjectJonesVectorToTriangleAxesAndAdd(JonesVector& jones_vector) {
        OrientedJonesVector oriented_jones_vector(jones_vector);
        mElectricField[0] += oriented_jones_vector * mLocalAxis1;
        mElectricField[1] += oriented_jones_vector * mLocalAxis2;
        mElectricField[2] += oriented_jones_vector * mNormal;
    }

    real_number ComputeRMSElectricFieldIntensityFromLocalAxesComponents() const {
        return SQRT_OF_2_OVER_2 * std::sqrt(abs(mElectricField[0])*abs(mElectricField[0]) + abs(mElectricField[1])*abs(mElectricField[1]) + abs(mElectricField[2])*abs(mElectricField[2]));
    }


};

#endif