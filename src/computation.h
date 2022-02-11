#ifndef __Ray_it__Computation__
#define __Ray_it__Computation__

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include "antenna.h"
#include "constants.h"
#include "inputs_reader.h"
#include "outputs_writer.h"
#include "mesh.h"
#include "../external_libraries/json.hpp"
#include <random>

using namespace nlohmann;

class Computation {
public:

    Computation(){}

    std::knuth_b mRandomEngine;


    bool Run(const json& parameters) {

        std::vector<Antenna> antennas;

        InputsReader reader;
        if(reader.ReadAntennas(antennas, parameters)) return 1;

        Mesh mesh;
        if(reader.ReadTerrainMesh(mesh, parameters["terrain_input_settings"])) return 1;

        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Building KD-tree... " << std::endl;
        KDTreeNode* root= new KDTreeNode();
        mesh.mTree = *root->RecursiveTreeNodeBuild(mesh.mTriangles, Box(mesh.mBoundingBox[0], mesh.mBoundingBox[1]), 0, SplitPlane());
        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Finished building KD-tree."<< std::endl;

        if(ComputeRays(antennas, parameters["computation_settings"], mesh)) return 1;

        OutputsWriter writer;
        const std::string output_file_name_with_current_path = CURRENT_WORKING_DIR + "/" + parameters["case_name"].get<std::string>();
        if(parameters["output_settings"]["print_for_gid"].get<bool>()) {
            writer.PrintResultsInGidFormat(mesh, antennas, output_file_name_with_current_path, TypeOfResultsPrint::RESULTS_ON_ELEMENTS);
        }
        if(parameters["output_settings"]["print_for_matlab"].get<bool>()) {
            writer.PrintResultsInMatlabFormat(mesh, antennas, TypeOfResultsPrint::RESULTS_ON_ELEMENTS);
        }
        return 0;
    }

    bool random_bool_with_prob(std::uniform_real_distribution<>& uniform_distribution_zero_to_one, const real_number prob) {  // probability between 0.0 and 1.0
        return uniform_distribution_zero_to_one(mRandomEngine) < prob;
    }

    bool ComputeRays(const std::vector<Antenna>& antennas, const json& computation_settings, Mesh& mesh){
        const int number_of_reflexions = computation_settings["number_of_reflexions"].get<int>();
        const real_number portion_of_elements_contributing_to_reflexion =  computation_settings["portion_of_elements_contributing_to_reflexion"].get<double>();
        const real_number fresnel_reflexion_coefficient =  computation_settings["Fresnel_reflexion_coefficient"].get<double>();
        const real_number minimum_intensity_to_be_reflected = computation_settings["minimum_intensity_to_be_reflected"].get<double>();

        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Computation starts. Computing rays... "<<std::endl;

        std::vector<std::vector<std::vector<Antenna>>> bdrf_antennas;

        if(number_of_reflexions){
            bdrf_antennas.resize(number_of_reflexions);
            for(int reflexion_number=0; reflexion_number<number_of_reflexions; reflexion_number++) {
                bdrf_antennas[reflexion_number].resize(mesh.mTriangles.size());
            }
        }

        for(size_t antenna_index=0; antenna_index<antennas.size(); ++antenna_index) {
            Vec3 origin = antennas[antenna_index].mCoordinates;
            const real_number measuring_dist_squared = antennas[antenna_index].mRadiationPattern.mMeasuringDistance * antennas[antenna_index].mRadiationPattern.mMeasuringDistance;
            if(RAY_IT_ECHO_LEVEL > 0) std::cout<<"Antenna '"<<antennas[antenna_index].mName<<"' at position: "<<std::setprecision(15)<<origin<<std::endl;
            #pragma omp parallel for schedule(dynamic, 500)
            for(int i = 0; i<(int)mesh.mTriangles.size(); i++) {
                const auto& triangle = mesh.mTriangles[i];
                Vec3 vec_origin_to_triangle_center = Vec3(triangle->mCenter[0] - origin[0], triangle->mCenter[1] - origin[1], triangle->mCenter[2] - origin[2]);
                Ray ray(origin, vec_origin_to_triangle_center);
                ray.Intersect(mesh);
                const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                const real_number distance = sqrt(distance_squared);
                if(std::abs(ray.t_max - distance) < 1.0) {
                    const real_number E_phi_at_measuring_distance = antennas[antenna_index].DirectionalRMSPhiPolarizationElectricFieldValue(vec_origin_to_triangle_center);
                    const real_number E_theta_at_measuring_distance = antennas[antenna_index].DirectionalRMSThetaPolarizationElectricFieldValue(vec_origin_to_triangle_center);
                    const real_number power_density_times_impedance = (E_phi_at_measuring_distance*E_phi_at_measuring_distance + E_theta_at_measuring_distance*E_theta_at_measuring_distance) * measuring_dist_squared / distance_squared;
                    triangle->mIntensity = std::sqrt(power_density_times_impedance);

                    if (number_of_reflexions) {
                        const real_number total_power_received_by_triangle = power_density_times_impedance / 377.0 * triangle->ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle->mNormal);
                        #if RAY_IT_DEBUG
                        if(total_power_received_by_triangle < 0.0) {
                            std::cout<<"Error: negative power!"<<std::endl;
                        }
                        #endif
                        const real_number total_power_reflected_by_triangle = fresnel_reflexion_coefficient * fresnel_reflexion_coefficient * total_power_received_by_triangle; //squared coefficient because we are reflecting power
                        //isotropic radiation pattern:
                        Antenna a;
                        a.mRadiationPattern.mTotalPower = total_power_reflected_by_triangle;
                        a.mRadiationPattern.mMeasuringDistance = 1.0;
                        Vec3 vector_pointing_up = Vec3(triangle->p0[0] - triangle->mCenter[0], triangle->p0[1] - triangle->mCenter[1], triangle->p0[2] - triangle->mCenter[2]);
                        a.InitializeOrientation(triangle->mNormal, vector_pointing_up);
                        a.mRadiationPattern.mSeparationBetweenPhiValues = 60.0;
                        a.mRadiationPattern.mSeparationBetweenThetaValues = 60.0;
                        a.mRadiationPattern.mRadiationMap.resize(int(360.0 / a.mRadiationPattern.mSeparationBetweenPhiValues) + 1);
                        const real_number isotropic_power_density = a.mRadiationPattern.mTotalPower / (4.0 * M_PI * a.mRadiationPattern.mMeasuringDistance * a.mRadiationPattern.mMeasuringDistance);
                        for(size_t i=0;i<a.mRadiationPattern.mRadiationMap.size(); i++) {
                            a.mRadiationPattern.mRadiationMap[i].resize(int(180.0 / a.mRadiationPattern.mSeparationBetweenThetaValues) + 1);
                            for(size_t j=0; j<a.mRadiationPattern.mRadiationMap[i].size(); j++) {
                                a.mRadiationPattern.mRadiationMap[i][j].mGain = 0.0;
                                a.mRadiationPattern.mRadiationMap[i][j].mEPhi = std::sqrt(377.0 * isotropic_power_density);
                                a.mRadiationPattern.mRadiationMap[i][j].mETheta = 0.0;
                            }
                        }
                        bdrf_antennas[0][i].push_back(a);
                    }
                }
            }
        }

        if(number_of_reflexions){
            std::uniform_real_distribution<> uniform_distribution_zero_to_one(0.0, 1.0);

            const real_number representation_factor = 1.0 / portion_of_elements_contributing_to_reflexion;

            for(int reflexion_number=0; reflexion_number<number_of_reflexions; reflexion_number++) {
                if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Computing reflexion "<< reflexion_number + 1 << " ... "<<std::endl;
                #pragma omp parallel for schedule(dynamic, 500)
                for(int i = 0; i<(int)mesh.mTriangles.size(); i++) {
                    const auto& triangle = mesh.mTriangles[i];
                    for(int j = 0; j<(int)bdrf_antennas[reflexion_number].size(); j++) {
                        if(j == i) continue;
                        if(mesh.mTriangles[j]->mIntensity < minimum_intensity_to_be_reflected) continue;
                        if( ! random_bool_with_prob(uniform_distribution_zero_to_one, portion_of_elements_contributing_to_reflexion) ) continue;
                        if(Vec3::DotProduct(triangle->mNormal, mesh.mTriangles[j]->mNormal) < 0.0) continue;

                        Vec3 vec_origin_to_triangle_center = Vec3(triangle->mCenter[0] - mesh.mTriangles[j]->mCenter[0], triangle->mCenter[1] - mesh.mTriangles[j]->mCenter[1], triangle->mCenter[2] - mesh.mTriangles[j]->mCenter[2]);
                        Ray ray(mesh.mTriangles[j]->mCenter, vec_origin_to_triangle_center);
                        ray.Intersect(mesh);
                        const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                        const real_number distance = sqrt(distance_squared);
                        if(std::abs(ray.t_max - distance) < 1.0) {
                            for(int k=0; k<bdrf_antennas[reflexion_number][j].size(); k++) {
                                const real_number E_phi_at_measuring_distance = bdrf_antennas[reflexion_number][j][k].DirectionalRMSPhiPolarizationElectricFieldValue(vec_origin_to_triangle_center);
                                const real_number E_theta_at_measuring_distance = bdrf_antennas[reflexion_number][j][k].DirectionalRMSThetaPolarizationElectricFieldValue(vec_origin_to_triangle_center);
                                const real_number measuring_dist_squared = bdrf_antennas[reflexion_number][j][k].mRadiationPattern.mMeasuringDistance * bdrf_antennas[reflexion_number][j][k].mRadiationPattern.mMeasuringDistance;
                                const real_number power_density_times_impedance = (E_phi_at_measuring_distance*E_phi_at_measuring_distance + E_theta_at_measuring_distance*E_theta_at_measuring_distance) * measuring_dist_squared / distance_squared;
                                triangle->mIntensity += representation_factor * std::sqrt(power_density_times_impedance);

                                if (number_of_reflexions > reflexion_number+1){
                                    bdrf_antennas[reflexion_number+1][i].push_back(Antenna());
                                }
                            }
                        }
                    }
                }
            }

        }

        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Computation finished."<<std::endl;

        return 0;
    }

};
#endif /* defined(__Ray_it__Computation__) */