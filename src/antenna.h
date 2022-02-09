#ifndef __Ray_it_antenna__
#define __Ray_it_antenna__

#include "constants.h"
#include "radiation_pattern.h"
#include "vector.h"
#include <math.h>

class Antenna {

    public:

    Antenna(){};


    std::string mName;
    Vec3 mCoordinates;
    RadiationPattern mRadiationPattern;
    Vec3 mVectorPointingFront;
    Vec3 mVectorPointingUp;
    Vec3 mVectorPointingLeft;


    void InitializeOrientation(const Vec3& front_direction, const Vec3& up_direction) {
        mVectorPointingFront = front_direction;
        mVectorPointingUp = up_direction;

        mVectorPointingFront.normalize();
        mVectorPointingUp.normalize();

        mVectorPointingLeft = Vec3::CrossProduct(mVectorPointingUp, mVectorPointingFront);
        mVectorPointingLeft.normalize();
    }


    Vec3 ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(const Vec3& global_direction) const {
        Vec3 local_dir;
        local_dir[0] = Vec3::DotProduct(global_direction, mVectorPointingFront);
        local_dir[1] = Vec3::DotProduct(global_direction, mVectorPointingLeft);
        local_dir[2] = Vec3::DotProduct(global_direction, mVectorPointingUp);
        return local_dir;
    }

    real_number DirectionalPowerValue(const Vec3& global_direction) const {
        Vec3 local_dir = ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(global_direction);
        real_number power_out = mRadiationPattern.mTotalPower * std::pow(10.0, 0.1* mRadiationPattern.DirectionalGainValue(local_dir));
        return power_out;
    }

    real_number DirectionalRMSPhiPolarizationElectricFieldValue(const Vec3& global_direction) const {
        Vec3 local_dir = ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(global_direction);
        return mRadiationPattern.DirectionalRMSPhiPolarizationElectricFieldValue(local_dir);
    }

    real_number DirectionalRMSThetaPolarizationElectricFieldValue(const Vec3& global_direction) const {
        Vec3 local_dir = ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(global_direction);
        return mRadiationPattern.DirectionalRMSThetaPolarizationElectricFieldValue(local_dir);
    }

    bool LoadRadiationPatternFromFile(const std::string& radiation_pattern_file_name) { //TODO: move this to the radiation pattern file
        mRadiationPattern.FillPatternInfoFrom4NEC2File(radiation_pattern_file_name);
        SphericalCoordinates any_orientation(0.0, 90.0);
        const real_number gain = mRadiationPattern.DirectionalGainValue(any_orientation);
        const real_number E_rms_phi = mRadiationPattern.DirectionalRMSPhiPolarizationElectricFieldValue(any_orientation);
        const real_number E_rms_theta = mRadiationPattern.DirectionalRMSThetaPolarizationElectricFieldValue(any_orientation);
        mRadiationPattern.mMeasuringDistance = sqrt(mRadiationPattern.mTotalPower * 120.0 * std::pow(10.0, gain * 0.1)/(4.0 * (E_rms_theta*E_rms_theta + E_rms_phi*E_rms_phi)));

        return 0;
    }

};

#endif