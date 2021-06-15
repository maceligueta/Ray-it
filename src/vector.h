#ifndef __BasicRaytracer__Vec3f__
#define __BasicRaytracer__Vec3f__
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <stdio.h>

class Vec3f{
public:
    float mCoordinates[3] = {0.0f, 0.0f, 0.0f};
    Vec3f(){}
    Vec3f(float x, float y, float z){
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

    inline Vec3f& normalize(){
        float inv_length = 1.0f / this->length();
        mCoordinates[0] *= inv_length;
        mCoordinates[1] *= inv_length;
        mCoordinates[2] *= inv_length;        
        return *this;
    };

    static Vec3f normalize(const Vec3f &v) {
        float inv_length = 1.0f / v.length();
        Vec3f result = Vec3f(v.X() * inv_length, v.Y() * inv_length, v.Z() * inv_length);
        return result;
    };

    static inline float DotProduct(const Vec3f &a, const Vec3f &b){
        return    (a.X() * b.X()
                 + a.Y() * b.Y()
                 + a.Z() * b.Z());

    };
    static inline Vec3f cross(const Vec3f &a, const Vec3f &b){
        return Vec3f(a.Y()*b.Z() - a.Z()*b.Y(),
                     a.Z()*b.X() - a.X()*b.Z(),
                     a.X()*b.Y() - a.Y()*b.X());
    };

    static inline float angle(const Vec3f &a, const Vec3f &b){
        return acos(DotProduct(a,b) / (a.length() * b.length()));
    };    

    inline Vec3f operator+(const Vec3f &other) const{
        return Vec3f(X()+other.X(), Y()+other.Y(), Z()+other.Z());
    };
    inline Vec3f operator-(const Vec3f &other) const{
        return Vec3f(X()-other.X(), Y()-other.Y(), Z()-other.Z());
    };

    inline Vec3f operator+=(const Vec3f &other){
        X() += other.X(), Y()+= other.Y(), Z() += other.Z();
        return *this;
    };

    inline Vec3f operator*(const Vec3f &other){
        return Vec3f(X()*other.X(), Y()*other.Y(), Z()*other.Z());
    };

    inline Vec3f operator*(const float scale) const{
        return Vec3f(X()*scale, Y()*scale, Z()*scale);
    };

    inline float operator[](const int axis) const{
        return mCoordinates[axis];
    }
    inline float &operator[](const int axis){
        return mCoordinates[axis];
    }
    friend std::ostream& operator<<(std::ostream& strm, const Vec3f &vec) {
        return strm << "[" << vec.X()  << vec.Y() << vec.Z() << "]";
    }
    
};
#endif /* defined(__BasicRaytracer__Vec3f__) */
