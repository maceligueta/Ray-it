#ifndef __Ray_it_antenna__
#define __Ray_it_antenna__

#include "constants.h"
#include "radiation_pattern.h"
#include "vector.h"
#include <math.h>
#include "../external_libraries/json.hpp"

using namespace nlohmann;

struct AntennaVariables {
    Vec3 mCoordinates;
    std::string mName;
    Vec3 mVectorPointingFront;
    Vec3 mVectorPointingUp;
    RadiationPattern mRadiationPattern;
};

class Antenna {

    public:

    //Antenna() = delete; //TODO: achieve this for robustness
    Antenna(){};
    Antenna(const AntennaVariables& antenna_vars){
        InitializeFromVariables(antenna_vars);
    }


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
                std::cout << "Error: files \""<<radiation_pattern_file_name<<"\" or \""<<file_name_with_current_path<<"\" not found! \n";
                return 1;
            }
            else{
            file_name_to_be_used_here = file_name_with_current_path;
            }
        }

        LoadRadiationPatternFrom4NEC2File(file_name_to_be_used_here);

        return 0;
    }

    bool InitializeFromVariables(const AntennaVariables& vars) {
        mName = vars.mName;
        mCoordinates = vars.mCoordinates;
        const Vec3 vector_pointing_front = vars.mVectorPointingFront;
        const Vec3 vector_pointing_up = vars.mVectorPointingUp;
        InitializeOrientation(vector_pointing_front, vector_pointing_up);
        mRadiationPattern = vars.mRadiationPattern;

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


    inline Vec3 ConvertGlobalDirIntoLocalDir(const Vec3& global_direction) const {
        Vec3 local_dir;
        local_dir[0] = Vec3::DotProduct(global_direction, mVectorPointingFront);
        local_dir[1] = Vec3::DotProduct(global_direction, mVectorPointingLeft);
        local_dir[2] = Vec3::DotProduct(global_direction, mVectorPointingUp);
        return local_dir;
    }

    inline OrientedJonesVector ConvertGlobalDirIntoLocalDir(const OrientedJonesVector& oriented_jones_vector) const {
        OrientedJonesVector oriented_jones_vector_in_local = OrientedJonesVector(oriented_jones_vector * mVectorPointingFront,
                                                                                 oriented_jones_vector * mVectorPointingLeft,
                                                                                 oriented_jones_vector * mVectorPointingUp,
                                                                                 oriented_jones_vector.mFrequency,
                                                                                 oriented_jones_vector.mReferenceDistance);
        return oriented_jones_vector_in_local;
    }

    inline Vec3 ConvertLocalDirIntoGlobalDir(const Vec3& local_direction) const {
        Vec3 global_dir;
        global_dir[0] = local_direction[0] * mVectorPointingFront[0] + local_direction[1] * mVectorPointingLeft[0] + local_direction[2] * mVectorPointingUp[0];
        global_dir[1] = local_direction[0] * mVectorPointingFront[1] + local_direction[1] * mVectorPointingLeft[1] + local_direction[2] * mVectorPointingUp[1];
        global_dir[2] = local_direction[0] * mVectorPointingFront[2] + local_direction[1] * mVectorPointingLeft[2] + local_direction[2] * mVectorPointingUp[2];
        return global_dir;
    }

    inline JonesVector GetDirectionalJonesVector(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        JonesVector local_jones_vector = mRadiationPattern.GetDirectionalJonesVector(local_dir);
        JonesVector global_jones_vector = local_jones_vector; //TODO: this variable can be removed for optimization
        global_jones_vector.mX = ConvertLocalDirIntoGlobalDir(local_jones_vector.mX);
        global_jones_vector.mY = ConvertLocalDirIntoGlobalDir(local_jones_vector.mY);
        return global_jones_vector;
    }

    inline real_number GetDirectionalPowerValue(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        const real_number power_out = mRadiationPattern.mTotalPower * std::pow(10.0, 0.1* mRadiationPattern.GetDirectionalGainValue(local_dir));
        return power_out;
    }

    inline real_number GetDirectionalRMSPhiPolarizationElectricFieldValue(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        return mRadiationPattern.GetDirectionalRMSPhiPolarizationElectricFieldValue(local_dir);
    }

    inline real_number GetDirectionalRMSThetaPolarizationElectricFieldValue(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        return mRadiationPattern.GetDirectionalRMSThetaPolarizationElectricFieldValue(local_dir);
    }

    inline Vec3 GetThetaTangentUnitaryVectorInGlobalCoordinates(const Vec3& global_direction){
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        const Vec3 theta_tangent_vector = mRadiationPattern.GetThetaTangentUnitaryVector(local_dir);
        return ConvertLocalDirIntoGlobalDir(theta_tangent_vector);
    }

    inline Vec3 GetPhiTangentUnitaryVectorInGlobalCoordinates(const Vec3& global_direction){
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        const Vec3 phi_tangent_vector = mRadiationPattern.GetPhiTangentUnitaryVector(local_dir);
        return ConvertLocalDirIntoGlobalDir(phi_tangent_vector);
    }

    inline bool LoadRadiationPatternFrom4NEC2File(const std::string& radiation_pattern_file_name) { //TODO: move this to the radiation pattern file
        mRadiationPattern.FillPatternInfoFrom4NEC2File(radiation_pattern_file_name);
        return 0;
    }

    inline void FillReflectedPatternInfoFromIncidentRay(const Vec3& incident_dir, const OrientedJonesVector& oriented_jones_vector, const Triangle& triangle) {
        const Vec3 local_incident_dir = ConvertGlobalDirIntoLocalDir(incident_dir);
        const OrientedJonesVector local_oriented_jones_vector = ConvertGlobalDirIntoLocalDir(oriented_jones_vector);
        const Vec3 local_triangle_normal = ConvertGlobalDirIntoLocalDir(triangle.mNormal);
        mRadiationPattern.FillReflectedPatternInfoFromIncidentRay(local_incident_dir, local_oriented_jones_vector, local_triangle_normal);
    }

    void AddContributionOfAnotherAntenna(const Antenna& other) {
        mRadiationPattern.mTotalPower += other.mRadiationPattern.mTotalPower;
        for(size_t i=0; i<mRadiationPattern.mRadiationMap.size(); i++) {
            for(size_t j=0; j<mRadiationPattern.mRadiationMap[i].size(); j++) {

                //obtain direction in local and global coordinates
                const SphericalCoordinates sp = mRadiationPattern.GetSphericalCoordinatesFromIndices(i, j);
                const Vec3 my_local_dir = sp.ConvertIntoCartesianCoordinates();
                const Vec3 global_dir = ConvertLocalDirIntoGlobalDir(my_local_dir);
                const JonesVector global_previous_jones_vector = GetDirectionalJonesVector(global_dir);
                const JonesVector global_jones_vector_to_be_added = other.GetDirectionalJonesVector(global_dir);
                const JonesVector to_remove = other.GetDirectionalJonesVector(Vec3(0.0, 0.0, 1.0));
                const JonesVector to_remove2 = GetDirectionalJonesVector(Vec3(0.0, 0.0, 1.0));
                const OrientedJonesVector sum = OrientedJonesVector(global_previous_jones_vector) + OrientedJonesVector(global_jones_vector_to_be_added);
                const JonesVector new_jones_vector = sum.ConvertToJonesVector(global_previous_jones_vector.mX, global_previous_jones_vector.mY);
                mRadiationPattern.mRadiationMap[i][j][ETheta] = new_jones_vector.mWaves[0].mAmplitude;
                mRadiationPattern.mRadiationMap[i][j][EThetaPhase] = new_jones_vector.mWaves[0].mPhase;
                mRadiationPattern.mRadiationMap[i][j][EPhi] = new_jones_vector.mWaves[1].mAmplitude;
                mRadiationPattern.mRadiationMap[i][j][EPhiPhase] = new_jones_vector.mWaves[1].mPhase;
            }
        }
        mRadiationPattern.SetGainValuesAccordingToElectricFieldValues();
    }

    inline Antenna operator+=(const Antenna& other){
        this->AddContributionOfAnotherAntenna(other);
        return *this;
    };

};

#endif