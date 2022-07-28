#ifndef __Ray_it_antenna__
#define __Ray_it_antenna__

#include "constants.h"
#include "radiation_pattern.h"
#include "vector.h"
#include <math.h>
#include <filesystem>
#include "../external_libraries/json.hpp"
#include "triangle.h"

using namespace nlohmann;
extern std::string CURRENT_WORKING_DIR;

struct AntennaVariables {
    Vec3 mCoordinates;
    std::string mName;
    Vec3 mVectorPointingFront;
    Vec3 mVectorPointingUp;
    std::shared_ptr<RadiationPattern> mRadiationPattern;
};

class Antenna {

    public:

    std::string mName;
    Vec3 mCoordinates;
    std::shared_ptr<RadiationPattern> mRadiationPattern;
    Vec3 mVectorPointingFront;
    Vec3 mVectorPointingUp;
    Vec3 mVectorPointingLeft;

    Antenna() {}

    Antenna(const Antenna& other) {
        mName = other.mName;
        mCoordinates = other.mCoordinates;
        mRadiationPattern = other.mRadiationPattern;
        mVectorPointingFront = other.mVectorPointingFront;
        mVectorPointingLeft = other.mVectorPointingLeft;
        mVectorPointingUp = other.mVectorPointingUp;
    }

    Antenna(const AntennaVariables& antenna_vars){
        InitializeFromVariables(antenna_vars);
    }

    Antenna(const json& single_antenna_settings) {
        InitializeFromParameters(single_antenna_settings);
    }

    ~Antenna() {}

    bool InitializeFromParameters(const json& single_antenna_settings);
    bool InitializeFromVariables(const AntennaVariables& vars);
    inline void InitializeOrientation(const Vec3& front_direction, const Vec3& up_direction);


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
        JonesVector local_jones_vector = mRadiationPattern->GetDirectionalJonesVector(local_dir);
        JonesVector global_jones_vector = local_jones_vector; //TODO: this variable can be removed for optimization
        global_jones_vector.mX = ConvertLocalDirIntoGlobalDir(local_jones_vector.mX);
        global_jones_vector.mY = ConvertLocalDirIntoGlobalDir(local_jones_vector.mY);
        return global_jones_vector;
    }

    inline real_number GetDirectionalPowerValue(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        const real_number power_out = mRadiationPattern->mTotalPower * real_number(std::pow(10.0, 0.1* mRadiationPattern->GetDirectionalGainValue(local_dir)));
        return power_out;
    }

    inline real_number GetDirectionalRMSPhiPolarizationElectricFieldValue(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        return mRadiationPattern->GetDirectionalRMSPhiPolarizationElectricFieldValue(local_dir);
    }

    inline real_number GetDirectionalRMSThetaPolarizationElectricFieldValue(const Vec3& global_direction) const {
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        return mRadiationPattern->GetDirectionalRMSThetaPolarizationElectricFieldValue(local_dir);
    }

    inline Vec3 GetThetaTangentUnitaryVectorInGlobalCoordinates(const Vec3& global_direction){
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        const Vec3 theta_tangent_vector = mRadiationPattern->GetThetaTangentUnitaryVector(local_dir);
        return ConvertLocalDirIntoGlobalDir(theta_tangent_vector);
    }

    inline Vec3 GetPhiTangentUnitaryVectorInGlobalCoordinates(const Vec3& global_direction){
        const Vec3 local_dir = ConvertGlobalDirIntoLocalDir(global_direction);
        const Vec3 phi_tangent_vector = mRadiationPattern->GetPhiTangentUnitaryVector(local_dir);
        return ConvertLocalDirIntoGlobalDir(phi_tangent_vector);
    }

    inline bool LoadRadiationPatternFrom4NEC2File(const std::string& radiation_pattern_file_name) { //TODO: move this to the radiation pattern file
        mRadiationPattern->FillPatternInfoFrom4NEC2File(radiation_pattern_file_name);
        return 0;
    }

    inline void FillReflectedPatternInfoFromIncidentRay(const Vec3& incident_dir, const OrientedJonesVector& oriented_jones_vector, const Vec3& normal_to_surface) {
        const Vec3 local_incident_dir = ConvertGlobalDirIntoLocalDir(incident_dir);
        const OrientedJonesVector local_oriented_jones_vector = ConvertGlobalDirIntoLocalDir(oriented_jones_vector);
        const Vec3 local_triangle_normal = ConvertGlobalDirIntoLocalDir(normal_to_surface);
        mRadiationPattern->FillReflectedPatternInfoFromIncidentRay(local_incident_dir, local_oriented_jones_vector, local_triangle_normal);
    }

    void AddContributionOfAnotherAntenna(const Antenna& other);

    inline Antenna operator+=(const Antenna& other){
        this->AddContributionOfAnotherAntenna(other);
        return *this;
    };


    private:

    inline JonesVector GetDirectionalJonesVector(const SphericalCoordinates& local_spherical_coordinates) const {
        JonesVector local_jones_vector = mRadiationPattern->GetDirectionalJonesVector(local_spherical_coordinates);
        JonesVector global_jones_vector = local_jones_vector; //TODO: this variable can be removed for optimization
        global_jones_vector.mX = ConvertLocalDirIntoGlobalDir(local_jones_vector.mX);
        global_jones_vector.mY = ConvertLocalDirIntoGlobalDir(local_jones_vector.mY);
        return global_jones_vector;
    }

};

#endif