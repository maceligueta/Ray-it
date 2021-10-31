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

    bool Run(json parameters) {

        std::vector<Antenna> antennas;

        InputsReader reader;
        if(reader.ReadAntennas(antennas, parameters)) return 1;

        Mesh mesh;
        if(reader.ReadTerrainMesh(mesh, parameters["terrain_input_settings"])) return 1;

        if(ComputeRays(antennas, mesh)) return 1;

        OutputsWriter writer;
        writer.PrintResultsInGidFormat(mesh, parameters["case_name"].get<std::string>(), TypeOfResultsPrint::RESULTS_ON_ELEMENTS);

        return 0;
    }

    bool ComputeRays(const std::vector<Antenna>& antennas, Mesh& mesh){

        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Computation starts. Computing rays... "<<std::endl;

        /* for(size_t i = 0; i<mesh.mNodes.size(); i++) {
            Vec3 vec_origin_to_node = Vec3(mesh.mNodes[i][0] - origin[0], mesh.mNodes[i][1] - origin[1], mesh.mNodes[i][2] - origin[2]);
            Ray test_ray(origin, vec_origin_to_node);
            test_ray.Intersect(mesh);
            const real_number distance_squared = vec_origin_to_node[0] * vec_origin_to_node[0] + vec_origin_to_node[1] *vec_origin_to_node[1] + vec_origin_to_node[2] * vec_origin_to_node[2];
            if(std::abs(test_ray.t_max * test_ray.t_max - distance_squared) < EPSILON) {
                mesh.mNodes[i].mIntensity = real_number(1.0) / distance_squared;
            }
        }  */

        for(size_t antenna_index=0; antenna_index<antennas.size(); ++antenna_index) {
            Vec3 origin = antennas[antenna_index].mCoordinates;
            std::cout<<"Antenna '"<<antennas[antenna_index].mName<<"' at position: "<<std::setprecision(15)<<origin<<std::endl;

            for(size_t i = 0; i<mesh.mTriangles.size(); i++) {
                Vec3 vec_origin_to_triangle_center = Vec3(mesh.mTriangles[i]->mCenter[0] - origin[0], mesh.mTriangles[i]->mCenter[1] - origin[1], mesh.mTriangles[i]->mCenter[2] - origin[2]);
                Ray test_ray(origin, vec_origin_to_triangle_center);
                test_ray.Intersect(mesh);
                const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                const real_number distance = sqrt(distance_squared);
                if(std::abs(test_ray.t_max - distance) < 1.0) {
                    mesh.mTriangles[i]->mIntensity = real_number(1.0) / distance_squared;
                    test_ray.mPower = mesh.mTriangles[i]->mIntensity * mesh.mTriangles[i]->ComputeArea() * Vec3::DotProduct(test_ray.mDirection, mesh.mTriangles[i]->mNormal);
                }
            }
        }

        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Computation finished."<<std::endl;

        return 0;
    }

};
#endif /* defined(__Ray_it__Computation__) */