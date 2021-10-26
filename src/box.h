#ifndef __BasicRayTracer__box_triangle__
#define __BasicRayTracer__box_triangle__
#include "vector.h"
#include "ray.h"
#include <stdio.h>
struct Box {
    Vec3 mMin;
    Vec3 mMax;
    Box(const Vec3 min, const Vec3 max): mMin(min), mMax(max){};
    Box(){};
    std::pair<real_number, real_number> Intersect(Ray &r) const;
    Vec3 GetBound(int type) const{
        if(type == 0) return mMin;
        return mMax;
    }
    real_number XWidth() const {
        return mMax.X() - mMin.X();
    };
    real_number YWidth() const {
        return mMax.Y() - mMin.Y();
    };
    real_number ZWidth() const {
        return mMax.Z() - mMin.Z();
    };
    real_number WidthAlongAxis(int axis) const {
        return mMax[axis] - mMin[axis];
    }
    real_number isPlanar(){
        return XWidth() <= 0.01 || YWidth() <= 0.01 || ZWidth() <= 0.01;
    }
};

#endif /* defined(__BasicRayTracer__box_triangle__) */
