#ifndef __Ray_it_radiation_pattern__
#define __Ray_it_radiation_pattern__

#include "constants.h"
#include "vector.h"
#include "jones.h"
#include "geometric_operations.h"
#include "math_utils.h"

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
        const real_number& x = (double)cartesian_direction[0];
        const real_number& y = (double)cartesian_direction[1];
        const real_number& z = (double)cartesian_direction[2];
        const real_number phi = real_number(std::atan2(y, x) * RAD_TO_DEG_FACTOR);
        const real_number theta = real_number(std::atan2(std::sqrt(x*x + y*y), z) * RAD_TO_DEG_FACTOR);
        new (this) SphericalCoordinates(phi, theta);
        #ifdef RAY_IT_DEBUG
        if(mPhi < -180 ||  mPhi > 180.0 || mTheta < 0.0 || mTheta > 180.0) {
            std::cout<<"WARNING: Spherical coordinates out of range when trying to get directional value in radiation pattern! \n";
        }
        #endif
    }

    Vec3 ConvertIntoCartesianCoordinates() const {
        const real_number phi_in_radians = mPhi * DEG_TO_RAD_FACTOR;
        const real_number theta_in_radians = mTheta * DEG_TO_RAD_FACTOR;
        return Vec3( sin(theta_in_radians)*cos(phi_in_radians), sin(theta_in_radians)*sin(phi_in_radians), cos(theta_in_radians));
    }
};

struct RadiationValues {
    real_number mValues[5];
    inline real_number& operator[](size_t i) {
        return mValues[i];
    }
    inline const real_number& operator[](size_t i) const {
        return mValues[i];
    }
};

typedef enum { Gain=0, EPhi=1, EPhiPhase=2, ETheta=3, EThetaPhase=4 } ValueType;


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

    inline SphericalCoordinates GetSphericalCoordinatesFromIndices(const size_t i, const size_t j) {
        return SphericalCoordinates(-180.0 + i * mSeparationBetweenPhiValues, j * mSeparationBetweenThetaValues);
    }

    inline real_number GetDirectionalGainValue(const Vec3& cartesian_direction) const {
        return GetDirectionalGainValue(SphericalCoordinates(cartesian_direction));
    }

    inline real_number GetDirectionalGainValue(const real_number& phi, const real_number& theta) const {
        return GetDirectionalGainValue(SphericalCoordinates(phi, theta));
    }

    inline real_number GetDirectionalValue(const ValueType variable, const SphericalCoordinates& spherical_coordinates) const {
        // phi must come [-180, 180] while theta must be [0, 180]
        #ifdef RAY_IT_DEBUG
        if(spherical_coordinates.mPhi < -180 ||  spherical_coordinates.mPhi > 180.0 || spherical_coordinates.mTheta < 0.0 || spherical_coordinates.mTheta > 180.0) {
            std::cout<<"WARNING: Spherical coordinates out of range when trying to get directional value in radiation pattern! \n";
        }
        #endif
        const real_number inv_sep_between_phi_vals = 1.0 / mSeparationBetweenPhiValues;
        const real_number inv_sep_between_theta_vals = 1.0 / mSeparationBetweenThetaValues;
        int floor_phi_index = (int)std::floor((spherical_coordinates.mPhi + real_number(180.0)) * inv_sep_between_phi_vals);
        if(std::abs(spherical_coordinates.mPhi - 180.0) < EPSILON) floor_phi_index--;
        int floor_theta_index = (int)std::floor(spherical_coordinates.mTheta * inv_sep_between_theta_vals);
        if(std::abs(spherical_coordinates.mTheta - 180.0) < EPSILON) floor_theta_index--;

        const real_number p_0_0 = mRadiationMap[floor_phi_index][floor_theta_index][variable];
        const real_number p_1_0 = mRadiationMap[floor_phi_index + 1][floor_theta_index][variable];
        const real_number p_0_1 = mRadiationMap[floor_phi_index][floor_theta_index + 1][variable];
        const real_number p_1_1 = mRadiationMap[floor_phi_index + 1][floor_theta_index + 1][variable];

        const real_number value_at_floor_theta = p_0_0 + (p_1_0 - p_0_0) * inv_sep_between_phi_vals * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number value_at_floor_theta_plus_one = p_0_1 + (p_1_1 - p_0_1) * inv_sep_between_phi_vals * (spherical_coordinates.mPhi - (floor_phi_index*mSeparationBetweenPhiValues - real_number(180.0)));
        const real_number final_value = value_at_floor_theta + (value_at_floor_theta_plus_one - value_at_floor_theta) * inv_sep_between_theta_vals * (spherical_coordinates.mTheta - floor_theta_index*mSeparationBetweenThetaValues);
        return final_value;
    }

    inline real_number GetDirectionalGainValue(const SphericalCoordinates& spherical_coordinates) const {
        return GetDirectionalValue(Gain, spherical_coordinates);
    }

    inline real_number GetDirectionalRMSPhiPolarizationElectricFieldValue(const real_number& phi, const real_number& theta) const {
        return GetDirectionalRMSPhiPolarizationElectricFieldValue(SphericalCoordinates(phi, theta));
    }

    inline real_number GetDirectionalPhiPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        return GetDirectionalValue(EPhi, spherical_coordinates);
    }

    inline real_number GetDirectionalRMSPhiPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        return SQRT_OF_2_OVER_2 * GetDirectionalPhiPolarizationElectricFieldValue(spherical_coordinates);
    }

    inline real_number GetDirectionalPhiPolarizationElectricFieldPhaseValue(const SphericalCoordinates& spherical_coordinates) const {
        return GetDirectionalValue(EPhiPhase, spherical_coordinates);
    }

    inline real_number GetDirectionalRMSThetaPolarizationElectricFieldValue(const real_number& phi, const real_number& theta) const {
        return GetDirectionalRMSThetaPolarizationElectricFieldValue(SphericalCoordinates(phi, theta));
    }

    inline real_number GetDirectionalThetaPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        return GetDirectionalValue(ETheta, spherical_coordinates);
    }

    inline real_number GetDirectionalRMSThetaPolarizationElectricFieldValue(const SphericalCoordinates& spherical_coordinates) const {
        return SQRT_OF_2_OVER_2 * GetDirectionalThetaPolarizationElectricFieldValue(spherical_coordinates);
    }

    inline real_number DirectionalThetaPolarizationElectricFieldPhaseValue(const SphericalCoordinates& spherical_coordinates) const {
        return GetDirectionalValue(EThetaPhase, spherical_coordinates);
    }

    inline JonesVector GetDirectionalJonesVector(const SphericalCoordinates& spherical_coordinates) const {
        Wave w1 = Wave(mFrequency, GetDirectionalThetaPolarizationElectricFieldValue(spherical_coordinates), DirectionalThetaPolarizationElectricFieldPhaseValue(spherical_coordinates), mMeasuringDistance);
        Wave w2 = Wave(mFrequency, GetDirectionalPhiPolarizationElectricFieldValue(spherical_coordinates), GetDirectionalPhiPolarizationElectricFieldPhaseValue(spherical_coordinates), mMeasuringDistance);
        Vec3 dirx = GetThetaTangentUnitaryVector(spherical_coordinates);
        Vec3 diry = GetPhiTangentUnitaryVector(spherical_coordinates);
        return JonesVector(w1, w2, dirx, diry);
    }

    inline JonesVector GetDirectionalJonesVector(const Vec3& cartesian_direction) const {
        return GetDirectionalJonesVector(SphericalCoordinates(cartesian_direction));
    }

    inline Vec3 GetThetaTangentUnitaryVector(const SphericalCoordinates& spherical_coordinates) const {
        const real_number theta_in_radians = spherical_coordinates.mTheta * DEG_TO_RAD_FACTOR;
        const real_number phi_in_radians = spherical_coordinates.mPhi * DEG_TO_RAD_FACTOR;
        Vec3 minus_theta_tangent(-cos(theta_in_radians) * cos(phi_in_radians),
                                 -cos(theta_in_radians) * sin(phi_in_radians),
                                 sin(theta_in_radians));
        return minus_theta_tangent;
    }

    inline Vec3 GetThetaTangentUnitaryVector(const Vec3& cartesian_direction) const {
        return GetThetaTangentUnitaryVector(SphericalCoordinates(cartesian_direction));
    }

    inline Vec3 GetPhiTangentUnitaryVector(const SphericalCoordinates& spherical_coordinates) const {
        const real_number phi_in_radians = spherical_coordinates.mPhi * DEG_TO_RAD_FACTOR;
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
                mRadiationMap[i][j][Gain] = -1e10;
            }
        }
        #endif

        for (size_t i=0; i<list_of_thetas.size(); ++i) {
            const SphericalCoordinates sc = SphericalCoordinates(list_of_phis[i], list_of_thetas[i]);
            const int theta_position_in_pattern_matrix = (int)round((sc.mTheta - min_theta)/ mSeparationBetweenThetaValues);
            const int phi_position_in_pattern_matrix = (int)round((sc.mPhi - min_phi)/ mSeparationBetweenPhiValues);
            if(phi_position_in_pattern_matrix == 0 || phi_position_in_pattern_matrix == mRadiationMap.size()-1){
                mRadiationMap[0][theta_position_in_pattern_matrix][Gain] = list_of_power_gains[i];
                mRadiationMap[0][theta_position_in_pattern_matrix][EPhi] = real_number(list_of_E_phi[i]);
                mRadiationMap[0][theta_position_in_pattern_matrix][EPhiPhase] = real_number(list_of_E_phi[i] * DEG_TO_RAD_FACTOR);
                mRadiationMap[0][theta_position_in_pattern_matrix][ETheta] = real_number(list_of_E_theta[i]);
                mRadiationMap[0][theta_position_in_pattern_matrix][EThetaPhase] = real_number(list_of_E_theta[i] * DEG_TO_RAD_FACTOR);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix][Gain] = real_number(list_of_power_gains[i]);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix][EPhi] = real_number(list_of_E_phi[i]);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix][EPhiPhase] = real_number(list_of_E_phi[i] * DEG_TO_RAD_FACTOR);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix][ETheta] = real_number(list_of_E_theta[i]);
                mRadiationMap[mRadiationMap.size()-1][theta_position_in_pattern_matrix][EThetaPhase] = real_number(list_of_E_theta[i] * DEG_TO_RAD_FACTOR);
            }
            else {
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix][Gain] = real_number(list_of_power_gains[i]);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix][EPhi] = real_number(list_of_E_phi[i]);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix][ETheta] = real_number(list_of_E_theta[i]);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix][EPhiPhase] = real_number(list_of_E_phi_phase[i] * DEG_TO_RAD_FACTOR);
                mRadiationMap[phi_position_in_pattern_matrix][theta_position_in_pattern_matrix][EThetaPhase] = real_number(list_of_E_theta_phase[i] * DEG_TO_RAD_FACTOR);
            }
        }

        const SphericalCoordinates any_orientation(0.0, 90.0);
        const real_number gain = GetDirectionalGainValue(any_orientation);
        const real_number E_rms_phi = GetDirectionalRMSPhiPolarizationElectricFieldValue(any_orientation);
        const real_number E_rms_theta = GetDirectionalRMSThetaPolarizationElectricFieldValue(any_orientation);
        mMeasuringDistance = sqrt(mTotalPower * 120.0 * std::pow(10.0, gain * 0.1)/(4.0 * (E_rms_theta*E_rms_theta + E_rms_phi*E_rms_phi)));

        #ifdef RAY_IT_DEBUG
        for (size_t i=0; i<mRadiationMap.size(); ++i) {
            for (size_t j=0; j<mRadiationMap[i].size(); ++j) {
                if (mRadiationMap[i][j][Gain] == -1e10){
                    std::cout<<"Error: undefined value in radiation map, coordinates "<<i<<", "<<j<<std::endl;
                }
            }
        }
        #endif
    }

    void FillReflectedPatternInfoFromIncidentRay(const Vec3& local_incident_dir, const OrientedJonesVector& local_oriented_jones_vector, const Vec3& local_triangle_normal) {
        const real_number n2 = 5.0; // TODO: refractive index of the material

        const real_number isotropic_power_density = mTotalPower / (4.0 * M_PI * mMeasuringDistance * mMeasuringDistance);
        const real_number wave_number = 2.0 * M_PI * mFrequency * INVERSE_OF_SPEED_OF_LIGHT;

        for(size_t i=0; i<mRadiationMap.size(); i++){
            const real_number theta = (-180.0 + i * mSeparationBetweenPhiValues);
            const real_number theta_rads = theta * DEG_TO_RAD_FACTOR;
            for(size_t j=0; j<mRadiationMap[i].size(); j++){
                const real_number phi = j * mSeparationBetweenThetaValues;
                const real_number phi_rads = phi * DEG_TO_RAD_FACTOR;
                const Vec3 sub_reflected_dir = Vec3(cos(phi_rads)*sin(theta_rads), sin(phi_rads)*sin(theta_rads), cos(theta_rads));

                Vec3 local_incident_s_axis = Vec3::CrossProduct(local_triangle_normal, -1.0 * local_incident_dir);
                if(local_incident_s_axis.Norm2() < EPSILON) {
                    local_incident_s_axis = Vec3::CrossProduct(sub_reflected_dir, local_triangle_normal);
                }
                local_incident_s_axis.Normalize();

                const Vec3 local_incident_p_axis = Vec3::CrossProduct(local_incident_s_axis, local_incident_dir);
                const std::complex<real_number> s_oriented_incident_E = local_oriented_jones_vector * local_incident_s_axis;
                const std::complex<real_number> p_oriented_incident_E = local_oriented_jones_vector * local_incident_p_axis;

                const Vec3 this_subreflexion_normal_to_surface = GeometricOperations::ComputeNormalFromIncidentAndReflected(local_incident_dir, sub_reflected_dir);
                const real_number incidence_angle_in_rads = Vec3::DotProduct(local_incident_dir, this_subreflexion_normal_to_surface);
                const real_number aux = std::sqrt(1.0-std::pow(sin(incidence_angle_in_rads)/n2, 2));
                const real_number fresnel_r_s = ( cos(incidence_angle_in_rads) - n2 * aux ) / ( cos(incidence_angle_in_rads) + n2 * aux);
                const real_number fresnel_r_p = (aux - n2 * cos(incidence_angle_in_rads)) / (aux + n2 * cos(incidence_angle_in_rads));

                Vec3 reflected_s_dir = Vec3::CrossProduct(sub_reflected_dir, local_triangle_normal);
                if(reflected_s_dir.Norm2() < EPSILON) {
                    reflected_s_dir = Vec3::CrossProduct(sub_reflected_dir, -1.0 * local_incident_dir); //TODO: sure?
                }
                const Vec3 reflected_p_dir = Vec3::CrossProduct(reflected_s_dir, sub_reflected_dir);

                OrientedJonesVector local_reflected_jones_vector = OrientedJonesVector(fresnel_r_s * s_oriented_incident_E,
                                                                                fresnel_r_p * p_oriented_incident_E,
                                                                                reflected_s_dir,
                                                                                reflected_p_dir,
                                                                                local_oriented_jones_vector.mFrequency,
                                                                                local_oriented_jones_vector.mReferenceDistance);

                Vec3 dir_phi = GetThetaTangentUnitaryVector(SphericalCoordinates(phi, theta));
                Vec3 dir_theta = GetPhiTangentUnitaryVector(SphericalCoordinates(phi, theta));

                const real_number E_phi = std::abs(local_reflected_jones_vector * dir_phi);
                const real_number E_theta = std::abs(local_reflected_jones_vector * dir_theta);
                const real_number power_density = (E_phi * E_phi + E_theta * E_theta) * INVERSE_OF_IMPEDANCE_OF_FREE_SPACE;

                const real_number expected_power_density = isotropic_power_density * std::pow(real_number(10.0), mRadiationMap[i][j][Gain]*real_number(0.1));
                real_number correction_factor;
                if(power_density) {
                    correction_factor = expected_power_density / power_density;
                } else {
                    correction_factor = 0.0;
                }
                const real_number phase_phi_at_distance_one_meter = std::arg(E_phi) + wave_number * 1.0;
                const real_number phase_theta_at_distance_one_meter = std::arg(E_theta) + wave_number * 1.0;

                //PROJECT REFLECTED JONES TO THETA PHI UNITARY TANGENT VECTORS
                mRadiationMap[i][j][EPhi] = E_phi * correction_factor;
                mRadiationMap[i][j][ETheta] = E_theta * correction_factor;
                mRadiationMap[i][j][EPhiPhase] = phase_phi_at_distance_one_meter;
                mRadiationMap[i][j][EThetaPhase] = phase_theta_at_distance_one_meter;
            }
        }
    }

    double IntegratePatternSurfaceTotalPowerBasedOnGain() {
        #include "points_on_unit_sphere.h"
        const auto& p = POINT_COORDINATES_ON_SPHERE;
        const size_t num_points = p.size();
        const double weight_of_each_point = 4.0 * M_PI / num_points;
        const double isotropic_power_density = mTotalPower / (4.0 * M_PI * mMeasuringDistance * mMeasuringDistance);
        double integral = 0.0;
        for(int i=0; i<num_points; i++){
            Vec3 dir = Vec3(p[i][0], p[i][1], p[i][2]);
            const double gain = GetDirectionalGainValue(dir);
            const double power_density = isotropic_power_density * std::pow(real_number(10.0), gain * real_number(0.1));
            integral += power_density;
        }
        integral *= weight_of_each_point;

        return integral;
    }

    double IntegratePatternSurfaceTotalPowerBasedOnElectricField() {
        #include "points_on_unit_sphere.h"
        const auto& p = POINT_COORDINATES_ON_SPHERE;
        const size_t num_points = p.size();
        const double weight_of_each_point = 4.0 * M_PI / num_points;
        double integral = 0.0;
        for(int i=0; i<num_points; i++){
            Vec3 dir = Vec3(p[i][0], p[i][1], p[i][2]);
            const auto sp = SphericalCoordinates(dir);
            const double E_phi = GetDirectionalPhiPolarizationElectricFieldValue(sp);
            const double E_theta = GetDirectionalThetaPolarizationElectricFieldValue(sp);
            const double power_density = (E_phi*E_phi +  E_theta*E_theta) * INVERSE_OF_IMPEDANCE_OF_FREE_SPACE;
            integral += power_density;
        }
        integral *= weight_of_each_point;

        return integral;
    }

    void ScaleDirectionalGainForConsistentIntegral() {
        #ifdef RAY_IT_DEBUG
        if(mTotalPower < EPSILON) std::cout<<"WARNING: a null power Radiation pattern is being integrated \n";
        #endif
        real_number integral;
        integral = IntegratePatternSurfaceTotalPowerBasedOnGain();

        real_number correction_factor = mTotalPower / integral;
        real_number aux_log_correction_factor = 10.0 * log10(correction_factor);

        //scaling the pattern to make it consistent with the total power emitted
        for(size_t i=0; i<mRadiationMap.size(); i++){
            for(size_t j=0; j<mRadiationMap[i].size(); j++){
                mRadiationMap[i][j][Gain] += aux_log_correction_factor;
            }
        }
    }

    void SetGainValuesAccordingToElectricFieldValues() {
        const real_number total_power = IntegratePatternSurfaceTotalPowerBasedOnElectricField();
        const real_number isotropic_power_density = total_power / (4.0 * M_PI * mMeasuringDistance * mMeasuringDistance);
        const real_number inverse_isotropic_power_density = 1.0 / isotropic_power_density;

        for(size_t i=0; i<mRadiationMap.size(); i++){
            for(size_t j=0; j<mRadiationMap[i].size(); j++){
                const real_number E_phi = mRadiationMap[i][j][EPhi];
                const real_number E_theta = mRadiationMap[i][j][ETheta];
                const real_number power_density_for_this_direction = (E_phi*E_phi + E_theta*E_theta) * INVERSE_OF_IMPEDANCE_OF_FREE_SPACE;
                mRadiationMap[i][j][Gain] = 10.0 * log10(power_density_for_this_direction * inverse_isotropic_power_density);
            }
        }
    }

    inline  bool IsANumber(const std::string& s) {
        char* end = nullptr;
        double val = strtod(s.c_str(), &end);
        return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
    }

};




#endif