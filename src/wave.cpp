#include "wave.h"

using namespace std::literals::complex_literals;

std::complex<real_number> Wave::ToComplex() const {
    return mAmplitude * std::exp(1i * mPhase);
}

void Wave::PropagateDistance(const real_number& distance_increase) {
    mAmplitude *= mReferenceDistance / (mReferenceDistance + distance_increase);
    const real_number wave_number = 2.0 * M_PI * mFrequency * INVERSE_OF_SPEED_OF_LIGHT;
    mPhase += wave_number * distance_increase;
    mReferenceDistance += distance_increase;
}