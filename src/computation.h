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

using namespace nlohmann;

class Computation {
public:

    Computation(){}

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

        if(ComputeRays(antennas, mesh)) return 1;

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

    bool ComputeRays(const std::vector<Antenna>& antennas, Mesh& mesh){

        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Computation starts. Computing rays... "<<std::endl;

        for(size_t antenna_index=0; antenna_index<antennas.size(); ++antenna_index) {
            Vec3 origin = antennas[antenna_index].mCoordinates;
            const real_number measuring_dist_squared = antennas[antenna_index].mMeasuringDistance * antennas[antenna_index].mMeasuringDistance;
            if(RAY_IT_ECHO_LEVEL > 0) std::cout<<"Antenna '"<<antennas[antenna_index].mName<<"' at position: "<<std::setprecision(15)<<origin<<std::endl;
            #pragma omp parallel for schedule(dynamic, 500)
            for(int i = 0; i<(int)mesh.mTriangles.size(); i++) {
                Vec3 vec_origin_to_triangle_center = Vec3(mesh.mTriangles[i]->mCenter[0] - origin[0], mesh.mTriangles[i]->mCenter[1] - origin[1], mesh.mTriangles[i]->mCenter[2] - origin[2]);
                Ray test_ray(origin, vec_origin_to_triangle_center);
                test_ray.Intersect(mesh);
                const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                const real_number distance = sqrt(distance_squared);
                if(std::abs(test_ray.t_max - distance) < 1.0) {
                    const real_number E_phi_at_measuring_distance = antennas[antenna_index].DirectionalRMSPhiPolarizationElectricFieldValue(vec_origin_to_triangle_center);
                    const real_number E_theta_at_measuring_distance = antennas[antenna_index].DirectionalRMSThetaPolarizationElectricFieldValue(vec_origin_to_triangle_center);
                    mesh.mTriangles[i]->mIntensity = (E_phi_at_measuring_distance*E_phi_at_measuring_distance + E_theta_at_measuring_distance*E_theta_at_measuring_distance) * measuring_dist_squared / distance_squared;
                    //test_ray.mPower = mesh.mTriangles[i]->mIntensity * mesh.mTriangles[i]->ComputeArea() * Vec3::DotProduct(test_ray.mDirection, mesh.mTriangles[i]->mNormal);
                }
            }
        }

        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Computation finished."<<std::endl;

        return 0;
    }

};
#endif /* defined(__Ray_it__Computation__) */