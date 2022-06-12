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

    JonesVector() = delete;

    JonesVector(const Wave& w1, const Wave& w2, const Vec3& dirx, const Vec3& diry){
        mWaves[0] = w1;
        mWaves[1] = w2;
        mX = dirx;
        mY = diry;
    }

    inline void PropagateDistance(const real_number& distance) {
        mWaves[0].PropagateDistance(distance);
        mWaves[1].PropagateDistance(distance);
    }

    inline real_number ComputeElectricFieldIntensity() {
        return std::sqrt(mWaves[0].mAmplitude*mWaves[0].mAmplitude + mWaves[1].mAmplitude*mWaves[1].mAmplitude);
    }

    inline real_number ComputePowerDensity() {
        return (mWaves[0].mAmplitude*mWaves[0].mAmplitude + mWaves[1].mAmplitude*mWaves[1].mAmplitude) * INVERSE_OF_IMPEDANCE_OF_FREE_SPACE;
    }

    inline real_number ComputeRMSPowerDensity() {
        return SQRT_OF_2_OVER_2 * ComputePowerDensity();
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
    VecC3 mOrientedVectorSum;
    real_number mFrequency;
    real_number mReferenceDistance;

    OrientedJonesVector(VecC3 oriented_vector_sum, const real_number frequency, const real_number ref_dist);
    OrientedJonesVector(const JonesVector& jones_vector);
    OrientedJonesVector(const std::complex<real_number>& wave1, const std::complex<real_number>& wave2, const Vec3& orientation1, const Vec3& orientation2, const real_number frequency, const real_number ref_dist);
    OrientedJonesVector(const std::complex<real_number>& wave1, const std::complex<real_number>& wave2, const std::complex<real_number>& wave3, const real_number frequency, const real_number ref_dist);

    JonesVector ConvertToJonesVector(const Vec3& first_dir, const Vec3& second_dir) const;
    const real_number ComputePowerDensity();

    inline std::complex<real_number> operator*(const Vec3& vector) const {
        return mOrientedVectorSum * vector;
    }

    inline OrientedJonesVector operator+(const OrientedJonesVector& other) const {
        #ifdef RAY_IT_DEBUG
        if(mFrequency != other.mFrequency) std::cout<<"WARNING: Trying to sum OrientedJonesVector with different frequencies! \n";
        if(mReferenceDistance != other.mReferenceDistance) std::cout<<"WARNING: Trying to sum OrientedJonesVector with reference distances for propagation! \n";
        #endif
        return OrientedJonesVector(this->mOrientedVectorSum + other.mOrientedVectorSum, mFrequency, mReferenceDistance);
    }

};

#endif