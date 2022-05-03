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

    OrientedJonesVector(VecC3 oriented_vector_sum, const real_number frequency, const real_number ref_dist) {
        mOrientedVectorSum = oriented_vector_sum;
        mFrequency = frequency;
        mReferenceDistance = ref_dist;
    }

    OrientedJonesVector(const JonesVector& jones_vector) {
        mOrientedVectorSum = jones_vector.mWaves[0].ToComplex() * jones_vector.mX + jones_vector.mWaves[1].ToComplex() * jones_vector.mY;
        mFrequency = jones_vector.mWaves[0].mFrequency;
        mReferenceDistance = jones_vector.mWaves[0].mReferenceDistance;
        #ifdef RAY_IT_DEBUG
        if(mFrequency != jones_vector.mWaves[1].mFrequency) std::cout<<"WARNING: JonesVector mixing frequencies! \n";
        if(mReferenceDistance != jones_vector.mWaves[1].mReferenceDistance) std::cout<<"WARNING: JonesVector mixing reference distances for propagation! \n";
        #endif
    }

    OrientedJonesVector(const std::complex<real_number>& wave1, const std::complex<real_number>& wave2, const Vec3& orientation1, const Vec3& orientation2, const real_number frequency, const real_number ref_dist) {
        mOrientedVectorSum = wave1 * orientation1 + wave2 * orientation2;
        mFrequency = frequency;
        mReferenceDistance = ref_dist;
    }

    OrientedJonesVector(const std::complex<real_number>& wave1, const std::complex<real_number>& wave2, const std::complex<real_number>& wave3, const real_number frequency, const real_number ref_dist) {
        mOrientedVectorSum[0] = wave1;
        mOrientedVectorSum[1] = wave2;
        mOrientedVectorSum[2] = wave3;
        mFrequency = frequency;
        mReferenceDistance = ref_dist;
    }

    JonesVector ConvertToJonesVector(const Vec3& first_dir, const Vec3& second_dir) const {
        std::complex<real_number> projection_to_first_dir = *this * first_dir;
        Wave w1(mFrequency, std::abs(projection_to_first_dir), std::arg(projection_to_first_dir), mReferenceDistance);
        std::complex<real_number> projection_to_second_dir = *this * second_dir;
        Wave w2(mFrequency, std::abs(projection_to_second_dir), std::arg(projection_to_second_dir), mReferenceDistance);

        #ifdef RAY_IT_DEBUG
        Vec3 third_dir = Vec3::CrossProduct(first_dir, second_dir);
        std::complex<real_number> projection_to_third_dir = *this * third_dir;
        Wave w3(mFrequency, std::abs(projection_to_third_dir), std::arg(projection_to_third_dir), mReferenceDistance);
        if(w3.mAmplitude > EPSILON) std::cout<<"WARNING: When converting an OrientedJonesVector into a JonesVector, a significant magnitude appeared in the direction of a third axis. A JonesVector should have information in just two axes. \n";
        #endif

        return JonesVector(w1, w2, first_dir, second_dir);
    }

    const real_number ComputePowerDensity() {
        const real_number mod = std::abs(mOrientedVectorSum[0]) * std::abs(mOrientedVectorSum[0]) +
                                std::abs(mOrientedVectorSum[1]) * std::abs(mOrientedVectorSum[1]) +
                                std::abs(mOrientedVectorSum[2]) * std::abs(mOrientedVectorSum[2]);
        return mod * INVERSE_OF_IMPEDANCE_OF_FREE_SPACE;
    }

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