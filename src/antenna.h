#ifndef __Ray_it_antenna__
#define __Ray_it_antenna__

#include "constants.h"
#include "radiation_pattern.h"
#include "vector.h"
#include <math.h>

using namespace nlohmann;

class Antenna {

    public:

    Antenna(){};


    std::string mName;
    Vec3 mCoordinates;
    RadiationPattern mRadiationPattern;
    Vec3 mVectorPointingFront;
    Vec3 mVectorPointingUp;
    Vec3 mVectorPointingLeft;


    bool InitializeFromParameters(const json& single_antenna_data) {
        mName = single_antenna_data["name"].get<std::string>();

        auto coords = single_antenna_data["coordinates"].get<std::vector<real_number>>();
        mCoordinates[0] = coords[0];
        mCoordinates[1] = coords[1];
        mCoordinates[2] = coords[2];

        auto vector = single_antenna_data["orientation"]["front"].get<std::vector<real_number>>();
        Vec3 vector_pointing_front;
        vector_pointing_front[0] = vector[0];
        vector_pointing_front[1] = vector[1];
        vector_pointing_front[2] = vector[2];

        vector = single_antenna_data["orientation"]["up"].get<std::vector<real_number>>();
        Vec3 vector_pointing_up;
        vector_pointing_up[0] = vector[0];
        vector_pointing_up[1] = vector[1];
        vector_pointing_up[2] = vector[2];

        InitializeOrientation(vector_pointing_front, vector_pointing_up);

        const std::string radiation_pattern_file_name = single_antenna_data["radiation_pattern_file_name"].get<std::string>();
        std::string file_name_to_be_used_here = radiation_pattern_file_name;

        if (!std::filesystem::exists(radiation_pattern_file_name)) {
            const std::string file_name_with_current_path = CURRENT_WORKING_DIR + "/" + radiation_pattern_file_name;
            if (!std::filesystem::exists(file_name_with_current_path)) {
                std::cout << "Error: files \""<<radiation_pattern_file_name<<"\" or \""<<file_name_with_current_path<<"\" not found!"<<std::endl;
                return 1;
            }
            else{
            file_name_to_be_used_here = file_name_with_current_path;
            }
        }

        LoadRadiationPatternFromFile(file_name_to_be_used_here);

        return 0;
    }


    inline void InitializeOrientation(const Vec3& front_direction, const Vec3& up_direction) {
        mVectorPointingFront = front_direction;
        mVectorPointingUp = up_direction;

        mVectorPointingFront.Normalize();
        mVectorPointingUp.Normalize();

        mVectorPointingLeft= Vec3::CrossProduct(mVectorPointingUp, mVectorPointingFront);
        mVectorPointingLeft.Normalize();
    }


    inline Vec3 ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(const Vec3& global_direction) const {
        Vec3 local_dir;
        local_dir[0] = Vec3::DotProduct(global_direction, mVectorPointingFront);
        local_dir[1] = Vec3::DotProduct(global_direction, mVectorPointingLeft);
        local_dir[2] = Vec3::DotProduct(global_direction, mVectorPointingUp);
        return local_dir;
    }

    inline Vec3 ConvertLocalDirIntoGlobalDirAccordingToAntennaOrientation(const Vec3& local_direction) const {
        Vec3 global_dir;
        global_dir[0] = local_direction[0] * mVectorPointingFront[0] + local_direction[1] * mVectorPointingLeft[0] + local_direction[2] * mVectorPointingUp[0];
        global_dir[1] = local_direction[0] * mVectorPointingFront[1] + local_direction[1] * mVectorPointingLeft[1] + local_direction[2] * mVectorPointingUp[1];
        global_dir[2] = local_direction[0] * mVectorPointingFront[2] + local_direction[1] * mVectorPointingLeft[2] + local_direction[2] * mVectorPointingUp[2];
        return global_dir;
    }

    inline JonesVector GetDirectionalJonesVector(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(global_direction);
        JonesVector jones_vector = mRadiationPattern.GetDirectionalJonesVector(local_dir);
        jones_vector.mX = ConvertLocalDirIntoGlobalDirAccordingToAntennaOrientation(jones_vector.mX);
        jones_vector.mY = ConvertLocalDirIntoGlobalDirAccordingToAntennaOrientation(jones_vector.mY);
        return jones_vector;
    }

    inline real_number GetDirectionalPowerValue(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(global_direction);
        const real_number power_out = mRadiationPattern.mTotalPower * std::pow(10.0, 0.1* mRadiationPattern.DirectionalGainValue(local_dir));
        return power_out;
    }

    inline real_number GetDirectionalRMSPhiPolarizationElectricFieldValue(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(global_direction);
        return mRadiationPattern.GetDirectionalRMSPhiPolarizationElectricFieldValue(local_dir);
    }

    inline real_number GetDirectionalRMSThetaPolarizationElectricFieldValue(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(global_direction);
        return mRadiationPattern.GetDirectionalRMSThetaPolarizationElectricFieldValue(local_dir);
    }

    inline Vec3 GetThetaTangentUnitaryVectorInGlobalCoordinates(const Vec3& global_direction){
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(global_direction);
        const Vec3 theta_tangent_vector = mRadiationPattern.GetThetaTangentUnitaryVector(local_dir);
        return ConvertLocalDirIntoGlobalDirAccordingToAntennaOrientation(theta_tangent_vector);
    }

    inline Vec3 GetPhiTangentUnitaryVectorInGlobalCoordinates(const Vec3& global_direction){
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDirAccordingToAntennaOrientation(global_direction);
        const Vec3 phi_tangent_vector = mRadiationPattern.GetPhiTangentUnitaryVector(local_dir);
        return ConvertLocalDirIntoGlobalDirAccordingToAntennaOrientation(phi_tangent_vector);
    }

    inline bool LoadRadiationPatternFromFile(const std::string& radiation_pattern_file_name) { //TODO: move this to the radiation pattern file
        mRadiationPattern.FillPatternInfoFrom4NEC2File(radiation_pattern_file_name);
        const SphericalCoordinates any_orientation(0.0, 90.0);
        const real_number gain = mRadiationPattern.DirectionalGainValue(any_orientation);
        const real_number E_rms_phi = mRadiationPattern.GetDirectionalRMSPhiPolarizationElectricFieldValue(any_orientation);
        const real_number E_rms_theta = mRadiationPattern.GetDirectionalRMSThetaPolarizationElectricFieldValue(any_orientation);
        mRadiationPattern.mMeasuringDistance = sqrt(mRadiationPattern.mTotalPower * 120.0 * std::pow(10.0, gain * 0.1)/(4.0 * (E_rms_theta*E_rms_theta + E_rms_phi*E_rms_phi)));

        return 0;
    }

};

#endif