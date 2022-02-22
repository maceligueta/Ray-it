#ifndef __Ray_it_radiation_pattern__
#define __Ray_it_radiation_pattern__

#include "constants.h"
#include "vector.h"
#include "jones.h"

#include <math.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

class SphericalCoordinates {
    public:
    real_number mPhi;
    real_number mTheta;
    SphericalCoordinates() = delete;
    SphericalCoordinates(const real_number& phi, const real_number& theta) {
        // phi must come [-180, 180] while theta must be [0, 180]
        //TODO: check values are in range in debug version

        if(theta < 0.0) {
            mTheta = -theta;
            if(phi >= 180.0) mPhi = phi - real_number(180.0);
            else if(phi < -180.0) mPhi = real_number(180.0) + phi;
            else if(phi < 0.0) mPhi = phi + real_number(180.0);
            else mPhi = phi - real_number(180.0);
        }
        else {
            mTheta = theta;
            if(phi >= 180.0) mPhi = phi - real_number(360.0);
            else if(phi < -180.0) mPhi = real_number(360.0) + phi;
            else mPhi = phi;
        }
        #if RAY_IT_DEBUG
            if(mPhi < -180.0 || mPhi > 180.0) {
                std::cout<<"Phi coordinate out of range when creating spherical coordinates with input values: phi = "<<phi<<", theta = "<<theta<<". Value of phi: "<<mPhi<<std::endl;
            }
            if(mTheta < 0.0 || mTheta > 180.0) {
                std::cout<<"Theta coordinate out of range when creating spherical coordinates with input values: phi = "<<phi<<", theta = "<<theta<<". Value of theta: "<<mTheta<<std::endl;
            }
        #endif

    }
    SphericalCoordinates(const Vec3& cartesian_direction) {
        const double x = (double)cartesian_direction[0];
        const double y = (double)cartesian_direction[1];
        const double z = (double)cartesian_direction[2];
        mPhi = real_number(atan2(y, x) * 180.0 * M_1_PI);
        mTheta = real_number(atan2(std::sqrt(x*x + y*y), z) * 180.0 * M_1_PI);
    }
};

struct RadiationValues {
  real_number mGain;
  real_number mEPhi;
  real_number mEPhiPhase;
  real_number mETheta;
  real_number mEThetaPhase;
};


class RadiationPattern {

    public:
    real_number mTotalPower = 0.0;
    real_number mFrequency = 0.0;
    real_number mMeasuringDistance;
    std::vector<std::vector<RadiationValues>> mRadiationMap;
    real_number mSeparationBetweenPhiValues;
    real_number mSeparationBetweenThetaValues;
    RadiationPattern(){
        CheckConstantSpacingBetweenValues();
    }

    inline real_number DirectionalGainValue(const Vec3& cartesian_direction) const {
        return DirectionalGainValue(SphericalCoordinates(cartesian_direction));
    }

    inline real_number DirectionalGainValue(const real_number& phi, const real_number& theta) const {
        return DirectionalGainValue(SphericalCoordinates(phi, theta));
    }

    inline real_number DirectionalGainValue(const SphericalCoordinates& spherical_coordinates) const {
        int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) / mSeparationBetweenPhiValues);
        if(spherical_coordinates.mPhi==180.0) floor_phi_index--;
        int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues); //TODO: BUG when THeta = 180.0
        if(spherical_coordinates.mTheta==180.0) floor_theta_index--;
        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index].mGain;
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index].mGain;
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1].mGain;
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1].mGain;
        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    inline real_number GetDirectionalRMSPhiPolarizationElectricFieldValue(const real_number& phi, const real_number& theta) const {
        return GetDirectionalRMSPhiPolarizationElectricFieldValue(SphericalCoordinates(phi, theta));
    }

    inline real_number GetDirectionalPhiPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        // phi must come [-180, 180] while theta must be [0, 180]
        //TODO: check values are in range in debug version
        int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) / mSeparationBetweenPhiValues);
        if(spherical_coordinates.mPhi==180.0) floor_phi_index--;
        int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues); //TODO: BUG when THeta = 180.0
        if(spherical_coordinates.mTheta==180.0) floor_theta_index--;
        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index].mEPhi;
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index].mEPhi;
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1].mEPhi;
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1].mEPhi;
        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    inline real_number GetDirectionalRMSPhiPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        return SQRT_OF_2_OVER_2 * GetDirectionalPhiPolarizationElectricFieldValue(spherical_coordinates);
    }

    inline real_number GetDirectionalPhiPolarizationElectricFieldPhaseValue(const SphericalCoordinates& spherical_coordinates) const {
        // phi must come [-180, 180] while theta must be [0, 180]
        //TODO: check values are in range in debug version
        int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) / mSeparationBetweenPhiValues);
        if(spherical_coordinates.mPhi==180.0) floor_phi_index--;
        int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues); //TODO: BUG when THeta = 180.0
        if(spherical_coordinates.mTheta==180.0) floor_theta_index--;
        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index].mEThetaPhase;
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index].mEThetaPhase;
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1].mEThetaPhase;
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1].mEThetaPhase;
        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    inline real_number GetDirectionalRMSThetaPolarizationElectricFieldValue(const real_number& phi, const real_number& theta) const {
        return GetDirectionalRMSThetaPolarizationElectricFieldValue(SphericalCoordinates(phi, theta));
    }

    inline real_number GetDirectionalThetaPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        // phi must come [-180, 180] while theta must be [0, 180]
        //TODO: check values are in range in debug version
        int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) / mSeparationBetweenPhiValues);
        if(spherical_coordinates.mPhi==180.0) floor_phi_index--;
        int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues); //TODO: BUG when THeta = 180.0
        if(spherical_coordinates.mTheta==180.0) floor_theta_index--;
        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index].mETheta;
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index].mETheta;
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1].mETheta;
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1].mETheta;
        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    inline real_number GetDirectionalRMSThetaPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        return SQRT_OF_2_OVER_2 * GetDirectionalThetaPolarizationElectricFieldValue(spherical_coordinates);
    }

    inline real_number DirectionalThetaPolarizationElectricFieldPhaseValue(const SphericalCoordinates& spherical_coordinates) const {
        // phi must come [-180, 180] while theta must be [0, 180]
        //TODO: check values are in range in debug version
        int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) / mSeparationBetweenPhiValues);
        if(spherical_coordinates.mPhi==180.0) floor_phi_index--;
        int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues); //TODO: BUG when THeta = 180.0
        if(spherical_coordinates.mTheta==180.0) floor_theta_index--;
        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index].mEThetaPhase;
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index].mEThetaPhase;
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1].mEThetaPhase;
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1].mEThetaPhase;
        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    inline JonesVector GetDirectionalJonesVector(const SphericalCoordinates& spherical_coordinates) const {
        JonesVector jones_vector;
        jones_vector[0] = Wave(mFrequency, GetDirectionalThetaPolarizationElectricFieldValue(spherical_coordinates), DirectionalThetaPolarizationElectricFieldPhaseValue(spherical_coordinates), mMeasuringDistance);
        jones_vector[1] = Wave(mFrequency, GetDirectionalPhiPolarizationElectricFieldValue(spherical_coordinates), GetDirectionalPhiPolarizationElectricFieldPhaseValue(spherical_coordinates), mMeasuringDistance);
        jones_vector.mX = GetThetaTangentUnitaryVector(spherical_coordinates);
        jones_vector.mY = GetPhiTangentUnitaryVector(spherical_coordinates);
        return jones_vector;
    }

    inline JonesVector GetDirectionalJonesVector(const Vec3& cartesian_direction) const {
        return GetDirectionalJonesVector(SphericalCoordinates(cartesian_direction));
    }

    inline Vec3 GetThetaTangentUnitaryVector(const SphericalCoordinates& spherical_coordinates) const {
        const real_number factor_degrees_to_radians = M_PI / 180.0;
        const real_number theta_in_radians = spherical_coordinates.mTheta * factor_degrees_to_radians;
        const real_number phi_in_radians = spherical_coordinates.mPhi * factor_degrees_to_radians;
        Vec3 minus_theta_tangent(-cos(theta_in_radians) * cos(phi_in_radians),
                                 -cos(theta_in_radians) * sin(phi_in_radians),
                                 sin(theta_in_radians));
        return minus_theta_tangent;
    }

    inline Vec3 GetThetaTangentUnitaryVector(const Vec3& cartesian_direction) const {
        return GetThetaTangentUnitaryVector(SphericalCoordinates(cartesian_direction));
    }

    inline Vec3 GetPhiTangentUnitaryVector(const SphericalCoordinates& spherical_coordinates) const {
        const real_number phi_in_radians = spherical_coordinates.mPhi * M_PI / 180.0;
        Vec3 minus_phi_tangent(sin(phi_in_radians), -cos(phi_in_radians), 0.0);
        return minus_phi_tangent;
    }

    inline Vec3 GetPhiTangentUnitaryVector(const Vec3& cartesian_direction) const {
        return GetPhiTangentUnitaryVector(SphericalCoordinates(cartesian_direction));
    }

    inline void CheckConstantSpacingBetweenValues(){}

    void FillPatternInfoFrom4NEC2File(const std::string& file_4NEC2_name) {
        std::ifstream antenna_file;
        std::string line;
        antenna_file.open(file_4NEC2_name);
        std::vector<real_number> list_of_thetas;
        std::vector<real_number> list_of_phis;
        std::vector<real_number> list_of_power_gains;
        std::vector<real_number> list_of_E_theta;
        std::vector<real_number> list_of_E_theta_phase;
        std::vector<real_number> list_of_E_phi;
        std::vector<real_number> list_of_E_phi_phase;
        real_number power = 0.0;

        while (getline(antenna_file, line)) {
            if (line.find("- - - POWER BUDGET - - -") != std::string::npos) {
                getline(antenna_file, line); //skipping line
                getline(antenna_file, line); //skipping line
                getline(antenna_file, line); //reading line with 'RADIATED POWER= 4.0874E+01 WATTS'
                std::stringstream line_stream(line); // include <sstream>
                std::string token;
                while (line_stream >> token) {
                    if (IsANumber(token)) {
                        power = real_number(std::stod(token));
                        break;
                    }
                }
                break;
            }
        }

        antenna_file.clear();
        antenna_file.seekg(0);

        real_number frequency_in_MHz = 0.0;
        while (getline(antenna_file, line)) {
            if (line.find("- - - - - - FREQUENCY - - - - - -") != std::string::npos) {
                getline(antenna_file, line); //skipping line
                getline(antenna_file, line); //reading line with 'FREQUENCY= 2.9979E+02 MHZ'
                std::stringstream line_stream(line); // include <sstream>
                std::string token;
                while (line_stream >> token) {
                    if (IsANumber(token)) {
                        frequency_in_MHz = real_number(std::stod(token));
                        break;
                    }
                }
                break;
            }
        }

        antenna_file.clear();
        antenna_file.seekg(0);

        while (getline(antenna_file, line)) {
            if (line.find("- - - RADIATION PATTERNS - - -") != std::string::npos) {
                getline(antenna_file, line); //skipping line
                getline(antenna_file, line); //skipping line
                getline(antenna_file, line); //skipping line
                getline(antenna_file, line); //skipping line
                getline(antenna_file, line); //first line with radiation pattern values
                while (!line.empty()) {
                    std::stringstream line_stream(line); // include <sstream>
                    std::string token;
                    line_stream >> token;
                    list_of_thetas.push_back(real_number(std::stod(token)));
                    line_stream >> token;
                    list_of_phis.push_back(real_number(std::stod(token)));
                    line_stream >> token; //skip one value
                    line_stream >> token; //skip one value
                    line_stream >> token;
                    list_of_power_gains.push_back(real_number(std::stod(token)));
                    line_stream >> token; //skip one value
                    line_stream >> token; //skip one value
                    line_stream >> token; //skip one value
                    line_stream >> token;
                    list_of_E_theta.push_back(real_number(std::stod(token)));
                    line_stream >> token;
                    list_of_E_theta_phase.push_back(real_number(std::stod(token)));
                    line_stream >> token;
                    list_of_E_phi.push_back(real_number(std::stod(token)));
                    line_stream >> token;
                    list_of_E_phi_phase.push_back(real_number(std::stod(token)));

                    getline(antenna_file, line);
                }
                break;
            }
        }

        mTotalPower = power;
        mFrequency = frequency_in_MHz * 1.0e6;
        mSeparationBetweenThetaValues = list_of_thetas[1] - list_of_thetas[0];
        mSeparationBetweenPhiValues = mSeparationBetweenThetaValues;

        mRadiationMap.resize(int(std::round(360.0 / mSeparationBetweenPhiValues)) + 1);
        for(size_t i=0; i<mRadiationMap.size(); ++i) {
            mRadiationMap[i].resize(int(std::round(180.0 / mSeparationBetweenThetaValues)) + 1);
        }

        real_number min_theta = 10000.0;
        real_number min_phi = 10000.0;

        for (size_t i=0; i<list_of_thetas.size(); ++i) {
            const SphericalCoordinates sc = SphericalCoordinates(list_of_phis[i], list_of_thetas[i]);
            if(sc.mTheta < min_theta) min_theta = sc.mTheta;
            if(sc.mPhi < min_phi) min_phi = sc.mPhi;
        }

        #ifdef RAY_IT_DEBUG
        for (size_t i=0; i<mRadiationMap.size(); ++i) {
            for (size_t j=0; j<mRadiationMap[i].size(); ++j) {
                mRadiationMap[i][j].mGain = -1e10;
            }
        }
        #endif

        const real_number factor_degrees_to_radians = M_PI / 180.0;

        for (size_t i=0; i<list_of_thetas.size(); ++i) {
            const SphericalCoordinates sc = SphericalCoordinates(list_of_phis[i], list_of_thetas[i]);
            const int theta_position_in_pattern_matrix = (int)round((sc.mTheta - min_theta)/ mSeparationBetweenThetaValues);
            const int phi_position_in_pattern_matrix = (int)round((sc.mPhi - min_phi)/ mSeparationBetweenPhiValues);
            if(phi_position_in_pattern_matrix == 0 || phi_position_in_pattern_matrix == mRadiationMap.size()-1){
                mRadiationMap[0][theta_position_in_pattern_matrix].mGain = list_of_power_gains[i];
                mRadiationMap[0][theta_position_in_pattern_matrix].mEPhi = real_number(list_of_E_phi[i]);
                mRadiationMap[0][theta_position_in_pattern_matrix].mEPhiPhase = real_number(list_of_E_phi[i] * factor_degrees_to_radians);
                mRadiationMap[0][theta_position_in_pattern_matrix].mETheta = real_number(list_of_E_theta[i]);
                mRadiationMap[0][theta_position_in_pattern_matrix].mEThetaPhase = real_number(list_of_E_theta[i] * factor_degrees_to_radians);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix].mGain = real_number(list_of_power_gains[i]);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix].mEPhi = real_number(list_of_E_phi[i]);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix].mEPhiPhase = real_number(list_of_E_phi[i] * factor_degrees_to_radians);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix].mETheta = real_number(list_of_E_theta[i]);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix].mEThetaPhase = real_number(list_of_E_theta[i] * factor_degrees_to_radians);
            }
            else {
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix].mGain = real_number(list_of_power_gains[i]);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix].mEPhi = real_number(list_of_E_phi[i]);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix].mETheta = real_number(list_of_E_theta[i]);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix].mEPhiPhase = real_number(list_of_E_phi_phase[i] * factor_degrees_to_radians);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix].mEThetaPhase = real_number(list_of_E_theta_phase[i] * factor_degrees_to_radians);
            }
        }

        #ifdef RAY_IT_DEBUG
        for (size_t i=0; i<mRadiationMap.size(); ++i) {
            for (size_t j=0; j<mRadiationMap[i].size(); ++j) {
                if (mRadiationMap[i][j].mGain == -1e10){
                    std::cout<<"Error: undefined value in radiation map, coordinates "<<i<<", "<<j<<std::endl;
                }
            }
        }
        #endif
    }

    double IntegratePatternSurfaceTotalPower() {
        #include "points_on_unit_sphere.h"
        const auto& p = POINT_COORDINATES_ON_SPHERE;
        const size_t num_points = p.size();
        const double weight_of_each_point = 4.0 * M_PI / num_points;
        const double isotropic_power_density = mTotalPower / (4.0 * M_PI * mMeasuringDistance * mMeasuringDistance);
        double integral = 0.0;
        for(int i=0; i<num_points; i++){
            Vec3 dir = Vec3(p[i][0], p[i][1], p[i][2]);
            const double gain = DirectionalGainValue(dir);
            const double power_density = isotropic_power_density * std::pow(real_number(10.0), gain * real_number(0.1));
            integral += power_density;
        }
        integral *= weight_of_each_point;

        return integral;
    }

    inline  bool IsANumber(const std::string& s) {
        char* end = nullptr;
        double val = strtod(s.c_str(), &end);
        return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
    }

};




#endif