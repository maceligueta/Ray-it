#ifndef __BasicRaytracer_radiation_patter__
#define __BasicRaytracer_radiation_patter__

#include "constants.h"

class SphericalCoordinates {
    public:
    real_number mPhi = 0.0;
    real_number mTheta = 0.0;
    SphericalCoordinates(){}
    SphericalCoordinates(const real_number& phi, const real_number& theta) {
        mPhi = phi;
        mTheta = theta;
    }
    SphericalCoordinates(const Vec3& cartesian_direction) {
        const double x = (double)cartesian_direction[0];
        const double y = (double)cartesian_direction[1];
        const double z = (double)cartesian_direction[2];
        mPhi = real_number(atan2(y, x) * 180.0 * M_1_PI);
        mTheta = real_number(atan2(std::sqrt(x*x + y*y), z) * 180.0 * M_1_PI);
    }
};


class RadiationPattern {

    public:
    std::vector<std::vector<real_number>> mRadiationMap;
    int mSeparationBetweenPhiValues;
    int mSeparationBetweenThetaValues;
    RadiationPattern(){
        CheckConstantSpacingBetweenValues();
    }


    real_number DirectionalPowerValue(const SphericalCoordinates& spherical_coordinates) {
        // phi must come [-180, 180] while theta must be [0, 180]
        //TODO: check values are in range in debug version
        const int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) / mSeparationBetweenPhiValues);
        const int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues);
        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index];
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index];
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1];
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1];
        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    void CheckConstantSpacingBetweenValues(){}

};




#endif