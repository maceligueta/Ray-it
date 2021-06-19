#ifndef __BasicRaytracer__Vec3f__
#define __BasicRaytracer__Vec3f__
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <stdio.h>

class Vec3{
public:
    float mCoordinates[3] = {0.0, 0.0, 0.0};
    Vec3(){}
    Vec3(float x, float y, float z){
        mCoordinates[0] = x;
        mCoordinates[1] = y;
        mCoordinates[2] = z;
    }
    inline float& X(){ return mCoordinates[0];}
    const inline float& X() const { return mCoordinates[0];}
    inline float& Y(){ return mCoordinates[1];}
    const inline float& Y() const { return mCoordinates[1];}
    inline float& Z(){ return mCoordinates[2];}
    const inline float& Z() const { return mCoordinates[2];}
    inline float length(void) const{
        return std::sqrt(lengthSq());
    };
    inline float lengthSq(void) const {
        return X()*X() + Y()*Y() + Z()*Z();
    }

    inline Vec3& normalize(){
        float inv_length = 1.0f / this->length();
        mCoordinates[0] *= inv_length;
        mCoordinates[1] *= inv_length;
        mCoordinates[2] *= inv_length;
        return *this;
    };

    static Vec3 normalize(const Vec3 &v) {
        float inv_length = 1.0f / v.length();
        Vec3 result = Vec3(v.X() * inv_length, v.Y() * inv_length, v.Z() * inv_length);
        return result;
    };

    static inline float DotProduct(const Vec3 &a, const Vec3 &b){
        return    (a.X() * b.X()
                 + a.Y() * b.Y()
                 + a.Z() * b.Z());

    };
    static inline Vec3 cross(const Vec3 &a, const Vec3 &b){
        return Vec3(a.Y()*b.Z() - a.Z()*b.Y(),
                     a.Z()*b.X() - a.X()*b.Z(),
                     a.X()*b.Y() - a.Y()*b.X());
    };

    static inline float angle(const Vec3 &a, const Vec3 &b){
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

    inline Vec3 operator*(const float scale) const{
        return Vec3(X()*scale, Y()*scale, Z()*scale);
    };

    inline float operator[](const int axis) const{
        return mCoordinates[axis];
    }
    inline float &operator[](const int axis){
        return mCoordinates[axis];
    }
    friend std::ostream& operator<<(std::ostream& strm, const Vec3 &vec) {
        return strm << "[" << vec.X()  << vec.Y() << vec.Z() << "]";
    }

};
#endif /* defined(__BasicRaytracer__Vec3f__) */
