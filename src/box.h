#ifndef __BasicRayTracer__box_triangle__
#define __BasicRayTracer__box_triangle__
#include "vector.h"
#include "ray.h"
#include <stdio.h>
struct Box {
    Vec3f mMin;
    Vec3f mMax;
    Box(const Vec3f min, const Vec3f max): mMin(min), mMax(max){};
    Box(){};
    std::pair<float, float> Intersect(Ray &r) const;
    Vec3f GetBound(int type) const{
        if(type == 0) return mMin;
        return mMax;
    }
    float XWidth() const {
        return mMax.X() - mMin.X();
    };
    float YWidth() const {
        return mMax.Y() - mMin.Y();
    };
    float ZWidth() const {
        return mMax.Z() - mMin.Z();
    };
    float WidthAlongAxis(int axis) const {
        return mMax[axis] - mMin[axis];
    }
    float isPlanar(){
        return XWidth() <= 0.01 || YWidth() <= 0.01 || ZWidth() <= 0.01;
    }
};

#endif /* defined(__BasicRayTracer__box_triangle__) */