#ifndef __BasicRaytracer__Vec3f__
#define __BasicRaytracer__Vec3f__
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <stdio.h>

#include "constants.h"

class Vec3{
public:
    real mCoordinates[3] = {0.0, 0.0, 0.0};
    Vec3(){}
    Vec3(real x, real y, real z){
        mCoordinates[0] = x;
        mCoordinates[1] = y;
        mCoordinates[2] = z;
    }
    inline real& X(){ return mCoordinates[0];}
    const inline real& X() const { return mCoordinates[0];}
    inline real& Y(){ return mCoordinates[1];}
    const inline real& Y() const { return mCoordinates[1];}
    inline real& Z(){ return mCoordinates[2];}
    const inline real& Z() const { return mCoordinates[2];}
    inline real length(void) const{
        return std::sqrt(lengthSq());
    };
    inline real lengthSq(void) const {
        return X()*X() + Y()*Y() + Z()*Z();
    }

    inline Vec3& normalize(){
        real inv_length = real(1.0) / this->length();
        mCoordinates[0] *= inv_length;
        mCoordinates[1] *= inv_length;
        mCoordinates[2] *= inv_length;
        return *this;
    };

    static Vec3 normalize(const Vec3 &v) {
        real inv_length = real(1.0) / v.length();
        Vec3 result = Vec3(v.X() * inv_length, v.Y() * inv_length, v.Z() * inv_length);
        return result;
    };

    static inline real DotProduct(const Vec3 &a, const Vec3 &b){
        return    (a.X() * b.X()
                 + a.Y() * b.Y()
                 + a.Z() * b.Z());

    };
    static inline Vec3 CrossProduct(const Vec3 &a, const Vec3 &b){
        return Vec3(a.Y()*b.Z() - a.Z()*b.Y(),
                     a.Z()*b.X() - a.X()*b.Z(),
                     a.X()*b.Y() - a.Y()*b.X());
    };

    static inline real angle(const Vec3 &a, const Vec3 &b){
        return acos(DotProduct(a,b) / (a.length() * b.length()));
    };

    inline Vec3 operator+(const Vec3 &other) const{
        return Vec3(X()+other.X(), Y()+other.Y(), Z()+other.Z());
    };
    inline Vec3 operator-(const Vec3 &other) const{
        return Vec3(X()-other.X(), Y()-other.Y(), Z()-other.Z());
    };

    inline Vec3 operator+=(const Vec3 &other){
        X() += other.X(), Y()+= other.Y(), Z() += other.Z();
        return *this;
    };

    inline Vec3 operator*(const Vec3 &other){
        return Vec3(X()*other.X(), Y()*other.Y(), Z()*other.Z());
    };

    inline Vec3 operator*(const real scale) const{
        return Vec3(X()*scale, Y()*scale, Z()*scale);
    };

    inline real operator[](const int axis) const{
        return mCoordinates[axis];
    }
    inline real &operator[](const int axis){
        return mCoordinates[axis];
    }
    friend std::ostream& operator<<(std::ostream& strm, const Vec3 &vec) {
        return strm << "[" << vec.X()<<"  "<< vec.Y() <<"  "<< vec.Z() << "]";
    }

};
#endif /* defined(__BasicRaytracer__Vec3f__) */
