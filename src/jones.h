#ifndef __Ray_it_jones__
#define __Ray_it_jones__

#include "constants.h"
#include "vector.h"
#include "wave.h"
#include <math.h>
#include <complex>

class JonesVector {
    public:
    Wave mWaves[2];
    Vec3 mX; // Vertical (Theta)
    Vec3 mY; // Horizontal (Phi)

    JonesVector(){};

    inline void PropagateDistance(const real_number distance) {
        mWaves[0].PropagateDistance(distance);
        mWaves[1].PropagateDistance(distance);
    }

    inline real_number ComputeElectricFieldIntensity() {
        return std::sqrt(mWaves[0].mAmplitude*mWaves[0].mAmplitude + mWaves[1].mAmplitude*mWaves[1].mAmplitude);
    }

    inline real_number ComputeRMSElectricFieldIntensity() {
        return SQRT_OF_2_OVER_2 * ComputeElectricFieldIntensity();
    }

    inline JonesVector operator =(const JonesVector &other) {
        mWaves[0] = other.mWaves[0];
        mWaves[1] = other.mWaves[1];
        mX = other.mX;
        mY = other.mY;
        return *this;
    }

    inline JonesVector operator *=(const real_number &scale) {
        mWaves[0] *= scale;
        mWaves[1] *= scale;
        return *this;
    }

    inline Wave& operator[](const int i) {
        return mWaves[i];
    }

};

class OrientedJonesVector {
    public:
    //JonesVector mJonesVector;
    VecC3 mOrientedWaves[2];
    VecC3 mOrientedVectorSum;

    OrientedJonesVector(JonesVector& jones_vector) {
        //mJonesVector = jones_vector;
        mOrientedWaves[0] = jones_vector.mWaves[0].ToComplex() * jones_vector.mX;
        mOrientedWaves[1] = jones_vector.mWaves[1].ToComplex() * jones_vector.mX;
        mOrientedVectorSum = mOrientedWaves[0] + mOrientedWaves[1];
    }


};

class JonesMatrix {

    public:
    std::complex<real_number> mValues[2][2];

    JonesMatrix(){};

    JonesMatrix(const std::complex<real_number>& a11, const std::complex<real_number>& a22, const std::complex<real_number>& a12, const std::complex<real_number>& a21) {
        mValues[0][0] = a11;
        mValues[1][1] = a22;
        mValues[0][1] = a12;
        mValues[1][0] = a21;
    }
};

#endif