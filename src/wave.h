#ifndef __Ray_it_wave__
#define __Ray_it_wave__

#include "constants.h"
#include "vector.h"
#include <math.h>
#include <complex>

class Wave {
    public:

    real_number mFrequency;
    real_number mAmplitude;
    real_number mPhase;
    real_number mReferenceDistance;

    Wave(){};
    Wave(const real_number& freq, const real_number& amp, const real_number& phase, const real_number& dist) {
        mFrequency = freq;
        mAmplitude = amp;
        mPhase = phase;
        mReferenceDistance = dist;
    }

    std::complex<real_number> ToComplex() const;

    void PropagateDistance(const real_number& distance_increase);

    inline Wave operator +(const Wave &other) const{
        std::complex<real_number> sum = this->ToComplex() + other.ToComplex();
        const real_number real_part = std::real(sum);
        const real_number imaginary_part = std::imag(sum);
        const real_number amplitude = std::sqrt(real_part*real_part + imaginary_part*imaginary_part);
        const real_number phase = std::atan2(imaginary_part, real_part);
        real_number dist = 0.0;
        if (mReferenceDistance == other.mReferenceDistance) {
            dist = mReferenceDistance;
        }
        return Wave(mFrequency, amplitude, phase, dist);
    }

    inline Wave operator =(const Wave &other) {
        mFrequency = other.mFrequency;
        mAmplitude = other.mAmplitude;
        mPhase = other.mPhase;
        mReferenceDistance = other.mReferenceDistance;
        return *this;
    }

    inline Wave operator+=(const Wave &other){
        *this = *this + other;
        return *this;
    }

    inline Wave operator*=(const real_number& scale){
        this->mAmplitude *= scale;
        return *this;
    }

    inline Wave operator*(const real_number& scale) const {
        Wave w = *this;
        return w *= scale;
    }

};

#endif