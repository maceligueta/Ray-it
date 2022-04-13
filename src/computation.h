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
#include "wave.h"
#include "jones.h"
#include "mesh.h"
#include "../external_libraries/json.hpp"
#include <random>
#include "geometric_operations.h"
#include "radiation_patterns/custom_radiation_patterns.h"

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

    bool RandomBoolAccordingToProbability(std::uniform_real_distribution<>& uniform_distribution_zero_to_one, const real_number prob) {  // probability between 0.0 and 1.0
        return uniform_distribution_zero_to_one(mRandomEngine) < prob;
    }

    bool ComputeRays(const std::vector<Antenna>& antennas, const json& computation_settings, Mesh& mesh){
        const int number_of_reflexions = computation_settings["number_of_reflexions"].get<int>();
        const real_number portion_of_elements_contributing_to_reflexion =  computation_settings["portion_of_elements_contributing_to_reflexion"].get<double>();
        const real_number fresnel_reflexion_coefficient =  computation_settings["Fresnel_reflexion_coefficient"].get<double>();
        const real_number minimum_intensity_to_be_reflected = computation_settings["minimum_intensity_to_be_reflected"].get<double>();

        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Computation starts. Computing rays... "<<std::endl;

        std::vector<std::vector<Antenna>> brdf_antennas;

        if(number_of_reflexions){
            brdf_antennas.resize(number_of_reflexions);
            for(int reflexion_number=0; reflexion_number<number_of_reflexions; reflexion_number++) {
                brdf_antennas[reflexion_number].resize(mesh.mTriangles.size());
                for(size_t i=0; i<mesh.mTriangles.size(); i++){
                    const Triangle& triangle = *mesh.mTriangles[i];
                    AntennaVariables empty_antenna_vars = AntennaVariables();
                    empty_antenna_vars.mCoordinates = triangle.mCenter;
                    empty_antenna_vars.mName = "";
                    empty_antenna_vars.mVectorPointingFront = triangle.mNormal;
                    empty_antenna_vars.mVectorPointingUp = (triangle.mP0 - triangle.mCenter).Normalize();
                    empty_antenna_vars.mRadiationPattern = BRDFDiffuseRadiationPattern(0.0, antennas[0].mRadiationPattern.mFrequency);
                    Antenna empty_brdf = Antenna(empty_antenna_vars);
                    brdf_antennas[reflexion_number][i] = empty_brdf;
                }

            }
        }

        #pragma omp parallel for schedule(dynamic, 500)
        for(int i = 0; i<(int)mesh.mTriangles.size(); i++) {
            Triangle& triangle = *mesh.mTriangles[i];
            for(size_t antenna_index=0; antenna_index<antennas.size(); ++antenna_index) {
                Vec3 origin = antennas[antenna_index].mCoordinates;
                const real_number measuring_dist_squared = antennas[antenna_index].mRadiationPattern.mMeasuringDistance * antennas[antenna_index].mRadiationPattern.mMeasuringDistance;
                Vec3 vec_origin_to_triangle_center = Vec3(triangle.mCenter[0] - origin[0], triangle.mCenter[1] - origin[1], triangle.mCenter[2] - origin[2]);
                Ray ray(origin, vec_origin_to_triangle_center);
                ray.Intersect(mesh);
                const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                const real_number distance = std::sqrt(distance_squared);
                if(std::abs(ray.t_max - distance) < 1.0) {
                    const JonesVector jones_vector_at_origin = antennas[antenna_index].GetDirectionalJonesVector(vec_origin_to_triangle_center);
                    JonesVector jones_vector_at_destination = jones_vector_at_origin;
                    jones_vector_at_destination.PropagateDistance(distance - antennas[antenna_index].mRadiationPattern.mMeasuringDistance);
                    triangle.ProjectJonesVectorToTriangleAxesAndAdd(jones_vector_at_destination);
                    triangle.mIntensity = triangle.ComputeRMSElectricFieldIntensityFromLocalAxesComponents();

                    if (number_of_reflexions) {
                        const real_number power_of_ray_received_by_triangle = jones_vector_at_destination.ComputeRMSPowerDensity() * triangle.ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle.mNormal);
                        #if RAY_IT_DEBUG
                        if(power_of_ray_received_by_triangle < 0.0) {
                            std::cout<<"Error: negative power!"<<std::endl;
                        }
                        #endif
                        const real_number power_of_ray_reflected_by_triangle = fresnel_reflexion_coefficient * fresnel_reflexion_coefficient * power_of_ray_received_by_triangle; //squared coefficient because we are reflecting power

                        // Build a BRDF at the reflection point (triangle center)
                        Vec3 reflection_dir = GeometricOperations::ComputeReflectionDirection(ray.mDirection, triangle.mNormal);
                        AntennaVariables antenna_vars = AntennaVariables();
                        antenna_vars.mCoordinates = triangle.mCenter;
                        antenna_vars.mName = "";
                        antenna_vars.mVectorPointingFront = reflection_dir;

                        if(Vec3::DotProduct(reflection_dir, triangle.mNormal) < 1.0-EPSILON) {
                            antenna_vars.mVectorPointingUp = Vec3::CrossProduct(reflection_dir, Vec3::CrossProduct(triangle.mNormal, reflection_dir));
                        }
                        else {
                            antenna_vars.mVectorPointingUp = triangle.mLocalAxis1;
                        }
                        antenna_vars.mRadiationPattern = BRDFDiffuseRadiationPattern(power_of_ray_reflected_by_triangle, jones_vector_at_destination.mWaves[0].mFrequency);

                        Antenna brdf = Antenna(antenna_vars);
                        brdf.FillReflectedPatternInfoFromIncidentRay(ray.mDirection, OrientedJonesVector(jones_vector_at_destination), triangle);

                        brdf_antennas[0][i] += brdf;
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
                    Triangle& triangle = *mesh.mTriangles[i];
                    for(int j = 0; j<(int)brdf_antennas[reflexion_number].size(); j++) {
                        if(j == i) continue;
                        if(mesh.mTriangles[j]->mIntensity < minimum_intensity_to_be_reflected) continue;
                        if( ! RandomBoolAccordingToProbability(uniform_distribution_zero_to_one, portion_of_elements_contributing_to_reflexion) ) continue;
                        if(Vec3::DotProduct(triangle.mNormal, mesh.mTriangles[j]->mNormal) < 0.0) continue;

                        Vec3 vec_origin_to_triangle_center = Vec3(triangle.mCenter[0] - mesh.mTriangles[j]->mCenter[0], triangle.mCenter[1] - mesh.mTriangles[j]->mCenter[1], triangle.mCenter[2] - mesh.mTriangles[j]->mCenter[2]);
                        Ray ray(mesh.mTriangles[j]->mCenter, vec_origin_to_triangle_center);
                        ray.Intersect(mesh);
                        const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                        const real_number distance = sqrt(distance_squared);

                        if(std::abs(ray.t_max - distance) < 1.0) {
                            const JonesVector jones_vector_at_origin = brdf_antennas[reflexion_number][j].GetDirectionalJonesVector(vec_origin_to_triangle_center);
                            JonesVector jones_vector_at_destination = jones_vector_at_origin;
                            jones_vector_at_destination.PropagateDistance(distance - brdf_antennas[reflexion_number][j].mRadiationPattern.mMeasuringDistance);
                            triangle.ProjectJonesVectorToTriangleAxesAndAdd(jones_vector_at_destination);
                            triangle.mIntensity = triangle.ComputeRMSElectricFieldIntensityFromLocalAxesComponents();

                            if (number_of_reflexions > reflexion_number+1){
                                const real_number power_of_ray_received_by_triangle = jones_vector_at_destination.ComputeRMSPowerDensity() * triangle.ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle.mNormal);
                                #if RAY_IT_DEBUG
                                if(power_of_ray_received_by_triangle < 0.0) {
                                    std::cout<<"Error: negative power!"<<std::endl;
                                }
                                #endif
                                const real_number power_of_ray_reflected_by_triangle = fresnel_reflexion_coefficient * fresnel_reflexion_coefficient * power_of_ray_received_by_triangle; //squared coefficient because we are reflecting power

                                // Build a BRDF at the reflection point (triangle center)
                                Vec3 reflection_dir = GeometricOperations::ComputeReflectionDirection(ray.mDirection, triangle.mNormal);
                                AntennaVariables antenna_vars = AntennaVariables();
                                antenna_vars.mCoordinates = triangle.mCenter;
                                antenna_vars.mName = "";
                                antenna_vars.mVectorPointingFront = reflection_dir;

                                if(Vec3::DotProduct(reflection_dir, triangle.mNormal) < 1.0-EPSILON) {
                                    antenna_vars.mVectorPointingUp = Vec3::CrossProduct(reflection_dir, Vec3::CrossProduct(triangle.mNormal, reflection_dir));
                                }
                                else {
                                    antenna_vars.mVectorPointingUp = triangle.mLocalAxis1;
                                }
                                antenna_vars.mRadiationPattern = BRDFDiffuseRadiationPattern(power_of_ray_reflected_by_triangle, jones_vector_at_destination.mWaves[0].mFrequency);

                                Antenna brdf = Antenna(antenna_vars);
                                brdf.FillReflectedPatternInfoFromIncidentRay(ray.mDirection, OrientedJonesVector(jones_vector_at_destination), triangle);
                                brdf_antennas[reflexion_number+1][i] += brdf;
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
#endif