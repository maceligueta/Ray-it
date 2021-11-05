#ifndef __Ray_it_radiation_patter__
#define __Ray_it_radiation_patter__

#include "constants.h"

#include <fstream>
#include <string>
#include <sstream>

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


class RadiationPattern {

    public:
    real_number mTotalPower = 0.0;
    std::vector<std::vector<std::vector<real_number>>> mRadiationMap;
    real_number mSeparationBetweenPhiValues;
    real_number mSeparationBetweenThetaValues;
    RadiationPattern(){
        CheckConstantSpacingBetweenValues();
    }

    real_number DirectionalGainValue(const Vec3& cartesian_direction) const {
        return DirectionalGainValue(SphericalCoordinates(cartesian_direction));
    }

    real_number DirectionalGainValue(const real_number& phi, const real_number& theta) const {
        return DirectionalGainValue(SphericalCoordinates(phi, theta));
    }

    real_number DirectionalGainValue(const SphericalCoordinates& spherical_coordinates) const {
        const int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) / mSeparationBetweenPhiValues);
        const int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues); //TODO: BUG when THeta = 180.0
        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index][0];
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index][0];
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1][0];
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1][0];
        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    real_number DirectionalRMSPhiPolarizationElectricFieldValue(const real_number& phi, const real_number& theta) const {
        return DirectionalRMSPhiPolarizationElectricFieldValue(SphericalCoordinates(phi, theta));
    }

    real_number DirectionalRMSPhiPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        // phi must come [-180, 180] while theta must be [0, 180]
        //TODO: check values are in range in debug version
        const int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) / mSeparationBetweenPhiValues);
        const int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues);
        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index][1];
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index][1];
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1][1];
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1][1];
        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    real_number DirectionalRMSThetaPolarizationElectricFieldValue(const real_number& phi, const real_number& theta) const {
        return DirectionalRMSThetaPolarizationElectricFieldValue(SphericalCoordinates(phi, theta));
    }

    real_number DirectionalRMSThetaPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        // phi must come [-180, 180] while theta must be [0, 180]
        //TODO: check values are in range in debug version
        const int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) / mSeparationBetweenPhiValues);
        const int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta/ mSeparationBetweenThetaValues);
        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index][2];
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index][2];
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1][2];
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1][2];
        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) / mSeparationBetweenPhiValues * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) / mSeparationBetweenThetaValues * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    void CheckConstantSpacingBetweenValues(){}

    void FillPatternInfoFrom4NEC2File(const std::string& file_4NEC2_name) {
        std::ifstream antenna_file;
        std::string line;
        antenna_file.open(file_4NEC2_name);
        std::vector<real_number> list_of_thetas;
        std::vector<real_number> list_of_phis;
        std::vector<real_number> list_of_power_gains;
        std::vector<real_number> list_of_E_theta;
        std::vector<real_number> list_of_E_phi;
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
                    line_stream >> token; //skip one value
                    line_stream >> token;
                    list_of_E_phi.push_back(real_number(std::stod(token)));

                    getline(antenna_file, line);
                }
                break;
            }
        }

        mTotalPower = power;
        mSeparationBetweenThetaValues = list_of_thetas[1] - list_of_thetas[0];
        mSeparationBetweenPhiValues = mSeparationBetweenThetaValues;

        mRadiationMap.resize(int(std::round(360.0 / mSeparationBetweenPhiValues)) + 1);
        for(size_t i=0; i<mRadiationMap.size(); ++i) {
            mRadiationMap[i].resize(int(std::round(180.0 / mSeparationBetweenThetaValues)) + 1);
            for(size_t j=0; j<mRadiationMap[i].size(); ++j) {
                mRadiationMap[i][j].resize(3);
            }
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
                mRadiationMap[i][j][0] = -1e10;
            }
        }
        #endif

        for (size_t i=0; i<list_of_thetas.size(); ++i) {
            const SphericalCoordinates sc = SphericalCoordinates(list_of_phis[i], list_of_thetas[i]);
            const int theta_position_in_pattern_matrix = (int)round((sc.mTheta - min_theta)/ mSeparationBetweenThetaValues);
            const int phi_position_in_pattern_matrix = (int)round((sc.mPhi - min_phi)/ mSeparationBetweenPhiValues);
            if(phi_position_in_pattern_matrix == 0 || phi_position_in_pattern_matrix == mRadiationMap.size()-1){
                mRadiationMap[0][theta_position_in_pattern_matrix][0] = list_of_power_gains[i];
                mRadiationMap[0][theta_position_in_pattern_matrix][1] = real_number(SQRT_OF_2_OVER_2 * list_of_E_phi[i]);
                mRadiationMap[0][theta_position_in_pattern_matrix][2] = real_number(SQRT_OF_2_OVER_2 * list_of_E_theta[i]);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix][0] = real_number(list_of_power_gains[i]);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix][1] = real_number(SQRT_OF_2_OVER_2 * list_of_E_phi[i]);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix][2] = real_number(SQRT_OF_2_OVER_2 * list_of_E_theta[i]);
            }
            else {
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix][0] = real_number(list_of_power_gains[i]);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix][1] = real_number(SQRT_OF_2_OVER_2 * list_of_E_phi[i]);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix][2] = real_number(SQRT_OF_2_OVER_2 * list_of_E_theta[i]);
            }
        }

        #ifdef RAY_IT_DEBUG
        for (size_t i=0; i<mRadiationMap.size(); ++i) {
            for (size_t j=0; j<mRadiationMap[i].size(); ++j) {
                if (mRadiationMap[i][j][0] == -1e10){
                    std::cout<<"Error: undefined value in radiation map, coordinates "<<i<<", "<<j<<std::endl;
                }
            }
        }
        #endif


    }

    bool IsANumber(const std::string& s) {
        char* end = nullptr;
        double val = strtod(s.c_str(), &end);
        return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
    }

};




#endif