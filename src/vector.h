#ifndef __Ray_it__Vec3f__
#define __Ray_it__Vec3f__
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <stdio.h>
#include <complex>

#include "constants.h"


template <class T>
class Vector3{
public:
    T mCoordinates[3] = {0.0, 0.0, 0.0};
    Vector3(){}
    Vector3(T x, T y, T z){
        mCoordinates[0] = x;
        mCoordinates[1] = y;
        mCoordinates[2] = z;
    }
    inline T& X(){ return mCoordinates[0];}
    const inline T& X() const { return mCoordinates[0];}
    inline T& Y(){ return mCoordinates[1];}
    const inline T& Y() const { return mCoordinates[1];}
    inline T& Z(){ return mCoordinates[2];}
    const inline T& Z() const { return mCoordinates[2];}
    inline T Length(void) const{
        return std::sqrt(LengthSq());
    };
    inline T LengthSq(void) const {
        return X()*X() + Y()*Y() + Z()*Z();
    }

    inline Vector3& Normalize(){
        T inv_length = T(1.0) / this->Length();
        mCoordinates[0] *= inv_length;
        mCoordinates[1] *= inv_length;
        mCoordinates[2] *= inv_length;
        return *this;
    };

    Vector3 Normalize() const {
        T inv_length = T(1.0) / this->Length();
        return Vector3(mCoordinates[0] * inv_length, mCoordinates[1] * inv_length, mCoordinates[2] * inv_length);
    }

    static inline Vector3 Normalize(const Vector3 &v) {
        T inv_length = T(1.0) / v.Length();
        Vector3 result = Vector3(v.X() * inv_length, v.Y() * inv_length, v.Z() * inv_length);
        return result;
    };

    static inline T Norm(const Vector3 &v) {
        return v.Length();
    }

    inline T Norm() const {
        return this->Length();
    }

    inline T Norm2() const {
        return this->LengthSq();
    }

    static inline T DotProduct(const Vector3 &a, const Vector3 &b){
        return    (a.X() * b.X()
                 + a.Y() * b.Y()
                 + a.Z() * b.Z());

    };
    static inline Vector3 CrossProduct(const Vector3 &a, const Vector3 &b){
        return Vector3(a.Y()*b.Z() - a.Z()*b.Y(),
                     a.Z()*b.X() - a.X()*b.Z(),
                     a.X()*b.Y() - a.Y()*b.X());
    };

    static inline T Angle(const Vector3 &a, const Vector3 &b){
        return acos(DotProduct(a,b) / (a.Length() * b.Length()));
    };

    inline Vector3 operator+(const Vector3 &other) const{
        return Vector3(X()+other.X(), Y()+other.Y(), Z()+other.Z());
    };
    inline Vector3 operator-(const Vector3 &other) const{
        return Vector3(X()-other.X(), Y()-other.Y(), Z()-other.Z());
    };

    inline Vector3& operator+=(const Vector3 &other){
        X() += other.X(), Y()+= other.Y(), Z() += other.Z();
        return *this;
    };

    inline Vector3 operator*(const Vector3 &other){
        return Vector3(X()*other.X(), Y()*other.Y(), Z()*other.Z());
    };

    inline Vector3 operator*(const T scale) const{
        return Vector3(X()*scale, Y()*scale, Z()*scale);
    };

    inline Vector3& operator*= (const real_number& multiplier) {
        X() *= multiplier;
        Y() *= multiplier;
        Z() *= multiplier;
        return *this;
    };

    inline T operator[](const int axis) const{
        return mCoordinates[axis];
    }
    inline T &operator[](const int axis){
        return mCoordinates[axis];
    }
    friend std::ostream& operator<<(std::ostream& strm, const Vector3 &vec) {
        return strm << "[" << vec.X()<<"  "<< vec.Y() <<"  "<< vec.Z() << "]";
    }

};

typedef Vector3<real_number> Vec3;
typedef Vector3<std::complex<real_number>> VecC3;

template <class T>
static inline Vector3<T> operator*(const real_number scale, const Vector3<T>& vector) {
    return vector * scale;
}

static inline VecC3 operator*(const std::complex<real_number> complex_scale, const Vec3& vector) {
    return VecC3(complex_scale * vector[0], complex_scale * vector[1], complex_scale * vector[2]);
}

static inline std::complex<real_number> operator*(const VecC3& complex_vector, const Vec3 vector) {
    return complex_vector[0] * vector[0] + complex_vector[1] * vector[1] + complex_vector[2] * vector[2];
}

static inline std::complex<real_number> operator*(const Vec3 vector, const VecC3& complex_vector) {
    return complex_vector[0] * vector[0] + complex_vector[1] * vector[1] + complex_vector[2] * vector[2];
}

inline std::complex<real_number> DotProduct(const VecC3& complex_vector, const Vec3 vector) {
    return complex_vector[0] * vector[0] + complex_vector[1] * vector[1] + complex_vector[2] * vector[2];
}

inline std::complex<real_number> DotProduct(const Vec3 vector, const VecC3& complex_vector) {
    return complex_vector[0] * vector[0] + complex_vector[1] * vector[1] + complex_vector[2] * vector[2];
}

#endif
