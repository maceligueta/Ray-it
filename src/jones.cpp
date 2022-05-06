#include "jones.h"

OrientedJonesVector::OrientedJonesVector(VecC3 oriented_vector_sum, const real_number frequency, const real_number ref_dist) {
    mOrientedVectorSum = oriented_vector_sum;
    mFrequency = frequency;
    mReferenceDistance = ref_dist;
}

OrientedJonesVector::OrientedJonesVector(const JonesVector& jones_vector) {
    mOrientedVectorSum = jones_vector.mWaves[0].ToComplex() * jones_vector.mX + jones_vector.mWaves[1].ToComplex() * jones_vector.mY;
    mFrequency = jones_vector.mWaves[0].mFrequency;
    mReferenceDistance = jones_vector.mWaves[0].mReferenceDistance;
    #ifdef RAY_IT_DEBUG
    if(mFrequency != jones_vector.mWaves[1].mFrequency) std::cout<<"WARNING: JonesVector mixing frequencies! \n";
    if(mReferenceDistance != jones_vector.mWaves[1].mReferenceDistance) std::cout<<"WARNING: JonesVector mixing reference distances for propagation! \n";
    #endif
}

OrientedJonesVector::OrientedJonesVector(const std::complex<real_number>& wave1, const std::complex<real_number>& wave2, const Vec3& orientation1, const Vec3& orientation2, const real_number frequency, const real_number ref_dist) {
    mOrientedVectorSum = wave1 * orientation1 + wave2 * orientation2;
    mFrequency = frequency;
    mReferenceDistance = ref_dist;
}

OrientedJonesVector::OrientedJonesVector(const std::complex<real_number>& wave1, const std::complex<real_number>& wave2, const std::complex<real_number>& wave3, const real_number frequency, const real_number ref_dist) {
    mOrientedVectorSum[0] = wave1;
    mOrientedVectorSum[1] = wave2;
    mOrientedVectorSum[2] = wave3;
    mFrequency = frequency;
    mReferenceDistance = ref_dist;
}

JonesVector OrientedJonesVector::ConvertToJonesVector(const Vec3& first_dir, const Vec3& second_dir) const {
    std::complex<real_number> projection_to_first_dir = *this * first_dir;
    Wave w1(mFrequency, std::abs(projection_to_first_dir), std::arg(projection_to_first_dir), mReferenceDistance);
    std::complex<real_number> projection_to_second_dir = *this * second_dir;
    Wave w2(mFrequency, std::abs(projection_to_second_dir), std::arg(projection_to_second_dir), mReferenceDistance);

    #ifdef RAY_IT_DEBUG
    Vec3 third_dir = Vec3::CrossProduct(first_dir, second_dir);
    std::complex<real_number> projection_to_third_dir = *this * third_dir;
    Wave w3(mFrequency, std::abs(projection_to_third_dir), std::arg(projection_to_third_dir), mReferenceDistance);
    if(w3.mAmplitude > 10 * EPSILON) {
        std::cout<<EPSILON<<"  "<<w3.mAmplitude<<"\n";
        std::cout<<"WARNING: When converting an OrientedJonesVector into a JonesVector, a significant magnitude appeared in the direction of a third axis. A JonesVector should have information in just two axes. \n";
    }
    #endif

    return JonesVector(w1, w2, first_dir, second_dir);
}

const real_number OrientedJonesVector::ComputePowerDensity() {
    const real_number mod = std::abs(mOrientedVectorSum[0]) * std::abs(mOrientedVectorSum[0]) +
                            std::abs(mOrientedVectorSum[1]) * std::abs(mOrientedVectorSum[1]) +
                            std::abs(mOrientedVectorSum[2]) * std::abs(mOrientedVectorSum[2]);
    return mod * INVERSE_OF_IMPEDANCE_OF_FREE_SPACE;
}


