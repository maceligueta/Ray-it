#include "antenna.h"

bool Antenna::InitializeFromParameters(const json& single_antenna_settings) {
    mName = single_antenna_settings["name"].get<std::string>();

    auto coords = single_antenna_settings["coordinates"].get<std::vector<real_number>>();
    mCoordinates[0] = coords[0];
    mCoordinates[1] = coords[1];
    mCoordinates[2] = coords[2];

    auto vector = single_antenna_settings["orientation"]["front"].get<std::vector<real_number>>();
    Vec3 vector_pointing_front;
    vector_pointing_front[0] = vector[0];
    vector_pointing_front[1] = vector[1];
    vector_pointing_front[2] = vector[2];

    vector = single_antenna_settings["orientation"]["up"].get<std::vector<real_number>>();
    Vec3 vector_pointing_up;
    vector_pointing_up[0] = vector[0];
    vector_pointing_up[1] = vector[1];
    vector_pointing_up[2] = vector[2];

    mRadiationPattern = std::make_shared<RadiationPattern>();

    InitializeOrientation(vector_pointing_front, vector_pointing_up);

    const std::string radiation_pattern_file_name = single_antenna_settings["radiation_pattern_file_name"].get<std::string>();
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

bool Antenna::InitializeFromVariables(const AntennaVariables& vars) {
    mName = vars.mName;
    mCoordinates = vars.mCoordinates;
    const Vec3 vector_pointing_front = vars.mVectorPointingFront;
    const Vec3 vector_pointing_up = vars.mVectorPointingUp;
    InitializeOrientation(vector_pointing_front, vector_pointing_up);
    mRadiationPattern = vars.mRadiationPattern;

    return 0;
}


inline void Antenna::InitializeOrientation(const Vec3& front_direction, const Vec3& up_direction) {
    mVectorPointingFront = front_direction;
    mVectorPointingUp = up_direction;

    mVectorPointingFront.Normalize();
    mVectorPointingUp.Normalize();

    mVectorPointingLeft= Vec3::CrossProduct(mVectorPointingUp, mVectorPointingFront);
    mVectorPointingLeft.Normalize();
}

void Antenna::AddContributionOfAnotherAntenna(const Antenna& other) {
    mRadiationPattern->mTotalPower += other.mRadiationPattern->mTotalPower;
    for(size_t i=0; i<mRadiationPattern->mRadiationMap.size(); i++) {
        for(size_t j=0; j<mRadiationPattern->mRadiationMap[i].size(); j++) {

            //obtain direction in local and global coordinates
            const SphericalCoordinates sp = mRadiationPattern->GetSphericalCoordinatesFromIndices(i, j);
            const JonesVector global_previous_jones_vector = GetDirectionalJonesVector(sp);
            const JonesVector global_jones_vector_to_be_added = other.GetDirectionalJonesVector(sp);
            const OrientedJonesVector a = OrientedJonesVector(global_previous_jones_vector);
            const OrientedJonesVector b = OrientedJonesVector(global_jones_vector_to_be_added);
            const OrientedJonesVector sum = a + b;
            const JonesVector new_jones_vector = sum.ConvertToJonesVector(global_previous_jones_vector.mX, global_previous_jones_vector.mY);
            mRadiationPattern->mRadiationMap[i][j][ETheta] = new_jones_vector.mWaves[0].mAmplitude;
            mRadiationPattern->mRadiationMap[i][j][EThetaPhase] = new_jones_vector.mWaves[0].mPhase;
            mRadiationPattern->mRadiationMap[i][j][EPhi] = new_jones_vector.mWaves[1].mAmplitude;
            mRadiationPattern->mRadiationMap[i][j][EPhiPhase] = new_jones_vector.mWaves[1].mPhase;
        }
    }
    mRadiationPattern->SetGainValuesAccordingToElectricFieldValues();
    //auto integral = mRadiationPattern->IntegratePatternSurfaceTotalPowerBasedOnElectricField(); //TODO: remove this or make it in DEBUG mode
}
