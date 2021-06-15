#ifndef __BasicRaytracer_radiation_patter__
#define __BasicRaytracer_radiation_patter__

#include <math.h>

class SphericalCoordinates {
    public:
    float mPhi = 0.0f;
    float mTheta = 0.0f;
    SphericalCoordinates(){}
    SphericalCoordinates(const float& phi, const float& theta) {
        mPhi = phi;
        mTheta = theta;
    }
    SphericalCoordinates(const Vec3f& cartesian_direction) {
        mPhi = std::atan2(cartesian_direction.Y(), cartesian_direction.X()) * 180.0f / M_PI;
        mTheta = std::atan2(std::sqrt(cartesian_direction.X()*cartesian_direction.X() + cartesian_direction.Y()*cartesian_direction.Y()), cartesian_direction.Z()) * 180.0f / M_PI;
    }
};


class RadiationPattern {

    public:
    std::vector<std::vector<float>> mRadiationMap;
    float mSeparationBetweenPhiValues;
    float mSeparationBetweenThetaValues;
    RadiationPattern(){
        CheckConstantSpacingBetweenValues();
    }
    

    float DirectionalPowerValue(const SphericalCoordinates& spherical_coordinates) {
        // phi must come [-180, 180] while theta must be [0, 180]
        //TODO: check values are in range in debug version
        const int floor_phi_index = std::floor((spherical_coordinates.mPhi + 180.0f) / mSeparationBetweenPhiValues);
        const int floor_theta_index = std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues);
        const float p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index];
        const float p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index];
        const float p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1];
        const float p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1];
        const float value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - 180.0f));
        const float value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - 180.0f));
        const float final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    void CheckConstantSpacingBetweenValues(){}
    
};




#endif