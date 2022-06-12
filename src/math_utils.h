#ifndef __Ray_it__Math_Utils__
#define __Ray_it__Math_Utils__
#include "constants.h"

static inline real_number atan_approximation(const real_number& x) {
    const real_number a1  = real_number( 0.99997726);
    const real_number a3  = real_number(-0.33262347);
    const real_number a5  = real_number( 0.19354346);
    const real_number a7  = real_number(-0.11643287);
    const real_number a9  = real_number( 0.05265332);
    const real_number a11 = real_number(-0.01172120);

    const real_number x_sq = x*x;
    return
        x * (a1 + x_sq * (a3 + x_sq * (a5 + x_sq * (a7 + x_sq * (a9 + x_sq * a11)))));
}

static real_number atan2_custom(const real_number y, const real_number x) {
        if(std::abs(x)<EPSILON && std::abs(y)<EPSILON) return 0.0;

        bool swap = fabs(x) < fabs(y);
        real_number atan_input = (swap ? x : y) / (swap ? y : x);
        real_number res = atan_approximation(atan_input);
        res = real_number( swap ? (atan_input >= 0.0f ? M_PI_2 : -M_PI_2) - res : res ); //TODO: NOT WORKING OPTIMIZATION. BE CAREFUL WITH y=-1 and x=0 for example
        //CHECK HERE: https://mazzo.li/posts/vectorized-atan2.html
        //CHECK HERE: https://pubs.opengroup.org/onlinepubs/9699919799/functions/atan2.html

        if      (x >= 0.0f && y >= 0.0f) {}                     // 1st quadrant
        else if (x <  0.0f && y >= 0.0f) { res = real_number(M_PI + res); } // 2nd quadrant
        else if (x <  0.0f && y <  0.0f) { res = real_number(-M_PI + res); } // 3rd quadrant
        else if (x >= 0.0f && y <  0.0f) {}                     // 4th quadrant

        return res;
}
#endif