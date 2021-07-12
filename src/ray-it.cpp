#include <stdio.h>
#include <vector>
#include <filesystem>

#include "constants.h"
#include "time.h"
#include "vector.h"
#include "mesh.h"
#include "ray.h"
#include "kdTree.h"
#include "tests_manager.h"
#include "gid_output.h"
#include "../external_libraries/timer.h"
#include "../external_libraries/stl_reader.h"
#include "../external_libraries/json.hpp"

using namespace nlohmann;


unsigned int echo_level = 1;


void PrintResultsInGidFormat(Mesh& mesh, const std::string& file_name, const TypeOfResultsPrint& print_type) {
    if(echo_level > 0) std::cout << "Printing results in GiD Post-process format... ";
    GidOutput gid_printer;
    gid_printer.PrintResults(mesh, file_name, print_type);
    if(echo_level > 0) std::cout << "  done!"<<std::endl;
}

bool ReadTerrainMesh(Mesh& mesh, const std::string& stl_file_name) {


    try {

        if (echo_level > 0) std::cout<<"Reading STL mesh ("<<stl_file_name<<")..."<<std::endl;
        stl_reader::StlMesh <real, unsigned int> stl_mesh (stl_file_name);

        if (echo_level > 1) {
            for(size_t itri = 0; itri < stl_mesh.num_tris(); ++itri) {
                std::cout << "Coords " << itri << ": ";
                for(size_t icorner = 0; icorner < 3; ++icorner) {
                    const real* c = stl_mesh.tri_corner_coords(itri, icorner);
                    std::cout << "(" << c[0] << ", " << c[1] << ", " << c[2] << ") ";
                }
                std::cout << std::endl;

                std::cout << "Indices " << itri << ": ( ";
                for(size_t icorner = 0; icorner < 3; ++icorner) {
                    const unsigned int c =  stl_mesh.tri_corner_ind (itri, icorner);
                    std::cout << c << " ";
                }
                std::cout << ")" << std::endl;

                const real* n = stl_mesh.tri_normal(itri);
                std::cout << "normal of triangle " << itri << ": "
                        << "(" << n[0] << ", " << n[1] << ", " << n[2] << ")\n";
            }
        }

        real xmin = INFINITY, ymin = INFINITY, zmin = INFINITY;
        real xmax = -INFINITY, ymax = -INFINITY, zmax = -INFINITY;

        for(int i=0; i<stl_mesh.num_vrts(); i++){
            Vec3 node(stl_mesh.vrt_coords(i)[0], stl_mesh.vrt_coords(i)[1], stl_mesh.vrt_coords(i)[2]);
            mesh.mNodes.push_back(node);
            xmin = fmin(xmin, node[0]);
            ymin = fmin(ymin, node[1]);
            zmin = fmin(zmin, node[2]);
            xmax = fmax(xmax, node[0]);
            ymax = fmax(ymax, node[1]);
            zmax = fmax(zmax, node[2]);
        }

        if (echo_level > 1) {
            std::cout << "List of nodes and coordinates: "<<std::endl;
            for(size_t i = 0; i < mesh.mNodes.size(); i++) {
                std::cout<<i<<"  "<<mesh.mNodes[i][0]<<"  "<<mesh.mNodes[i][1]<<"  "<<mesh.mNodes[i][2]<<std::endl;
            }
        }

        real added_tolerance = 0.001f * fmax( xmax-xmin, fmax(ymax-ymin, zmax-zmin));
        mesh.mBoundingBox[0] = Vec3(xmin-added_tolerance, ymin-added_tolerance, zmin-added_tolerance);
        mesh.mBoundingBox[1] = Vec3(xmax+added_tolerance, ymax+added_tolerance, zmax+added_tolerance);

        for(int i = 0; i < stl_mesh.num_tris(); i++){
            Vec3 N(stl_mesh.tri_normal(i)[0], stl_mesh.tri_normal(i)[1], stl_mesh.tri_normal(i)[2]);
            mesh.mNormals.push_back(N);

            Triangle* t = new Triangle(mesh);
            t->mNodeIndices[0] = stl_mesh.tri_corner_ind(i, 0);
            t->mNodeIndices[1] = stl_mesh.tri_corner_ind(i, 1);
            t->mNodeIndices[2] = stl_mesh.tri_corner_ind(i, 2);
            t->p0 = mesh.mNodes[t->mNodeIndices[0]];
            t->p1 = mesh.mNodes[t->mNodeIndices[1]];
            t->p2 = mesh.mNodes[t->mNodeIndices[2]];
            t->SetEdgesAndPrecomputedValues();
            t->mId = i;

            mesh.mTriangles.push_back(t);
        }

        if (echo_level > 0) std::cout<<"File "<<stl_file_name<<" was read correctly. "<<mesh.mNodes.size()<<" nodes and "<<mesh.mTriangles.size()<<" elements."<<std::endl;

        if(echo_level > 0) std::cout << "Building KD-tree... " << std::endl;
        KDTreeNode* root= new KDTreeNode();
        mesh.mTree = *root->RecursiveTreeNodeBuild(mesh.mTriangles, Box(mesh.mBoundingBox[0], mesh.mBoundingBox[1]), 0, SplitPlane());
        if(echo_level > 0) std::cout << "Finished building KD-tree."<< std::endl;

    } catch (std::exception& e) {
        std::cout<<e.what()<<std::endl;
        return false;
    }

    return true;
}

bool CheckInputParameters(json& input_parameters) {
    return true;
}

bool Compute(const std::vector<Antenna>& antennas, Mesh& mesh){

    if(echo_level > 0) std::cout << "Computation starts. Computing rays... "<<std::endl;

    /* for(size_t i = 0; i<mesh.mNodes.size(); i++) {
        Vec3 vec_origin_to_node = Vec3(mesh.mNodes[i][0] - origin[0], mesh.mNodes[i][1] - origin[1], mesh.mNodes[i][2] - origin[2]);
        Ray test_ray(origin, vec_origin_to_node);
        test_ray.Intersect(mesh);
        const real distance_squared = vec_origin_to_node[0] * vec_origin_to_node[0] + vec_origin_to_node[1] *vec_origin_to_node[1] + vec_origin_to_node[2] * vec_origin_to_node[2];
        if(std::abs(test_ray.t_max * test_ray.t_max - distance_squared) < EPSILON) {
            mesh.mNodes[i].mIntensity = real(1.0) / distance_squared;
        }
    }  */

    for(size_t antenna_index=0; antenna_index<antennas.size(); ++antenna_index) {
        Vec3 origin = antennas[antenna_index].mCoordinates;
        std::cout<<"Antenna '"<<antennas[antenna_index].mName<<"' at position: "<<std::setprecision(15)<<origin<<std::endl;

        for(size_t i = 0; i<mesh.mTriangles.size(); i++) {
            Vec3 vec_origin_to_triangle_center = Vec3(mesh.mTriangles[i]->mCenter[0] - origin[0], mesh.mTriangles[i]->mCenter[1] - origin[1], mesh.mTriangles[i]->mCenter[2] - origin[2]);
            Ray test_ray(origin, vec_origin_to_triangle_center);
            test_ray.Intersect(mesh);
            const real distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
            const real distance = sqrt(distance_squared);
            if(std::abs(test_ray.t_max - distance) < 1.0) {
                mesh.mTriangles[i]->mIntensity = real(1.0) / distance_squared;
                test_ray.mPower = mesh.mTriangles[i]->mIntensity * mesh.mTriangles[i]->ComputeArea() * Vec3::DotProduct(test_ray.mDirection, mesh.mTriangles[i]->mNormal);
            }
        }
    }

    if(echo_level > 0) std::cout << "Computation finished."<<std::endl;

    return true;
}

bool ReadInputParameters(const std::string& parameters_filename, json& input_parameters) {

    std::ifstream i(parameters_filename);
    if(echo_level > 0) std::cout << "Reading input parameters file ("<<parameters_filename<<")..."<<std::endl;
    if (!std::filesystem::exists(parameters_filename)) {
        std::cout << "Error: file \""<<parameters_filename<<"\" not found!"<<std::endl;
        return false;
    }
    try {
        i >> input_parameters;
    } catch (std::exception& e) {
        std::cout<<e.what()<<std::endl;
        return false;
    }

    CheckInputParameters(input_parameters);

    return true;
}

bool ReadAntennas(std::vector<Antenna>& antennas, nlohmann::json& input_parameters){

    for (auto& single_antenna_data : input_parameters["antennas_list"]) {
        Antenna a;
        a.mName = single_antenna_data["name"].get<std::string>();

        auto coords = single_antenna_data["coordinates"].get<std::vector<real>>();
        a.mCoordinates[0] = coords[0];
        a.mCoordinates[1] = coords[1];
        a.mCoordinates[2] = coords[2];

        antennas.push_back(a);
    }
    return true;
}

int main(int argc, char *argv[]) {

    //if(argc > 3 || argc < 2 || (argc == 2 && strcmp(argv[1], "tests") != 0) || (argc == 3 && strcmp(argv[1], "tests") == 0)) {
    if(argc!=2){
        std::cout<<"Error: wrong number of arguments. Type the argument 'tests' to run the tests, or type the paramters file informing about the input data (antennas and terrain) and settings."<<std::endl;
        return 1;
    }

    if(echo_level > 0) {
        std::cout << "Ray-it starting. Using ";
        #ifdef RAY_IT_USE_DOUBLES
        #pragma message("---- Compiling with double precision ----")
        std::cout << "double precision ('double' variables). ";
        #else
        #pragma message("---- Compiling with single precision ----")
        std::cout << "single precision ('float' variables). ";
        #endif
        std::cout<<std::endl<<std::endl;
    }

    Timer total_timer;
    total_timer.start();

    std::string stl_filename;
    std::string parameters_filename;

    if(strcmp(argv[1], "tests") == 0) {
        const int number_of_errors = RunTests();
        if (number_of_errors) return 1;
        else return 0;
    } else {
        parameters_filename = argv[1];
    }

    json parameters;

    if(!ReadInputParameters(parameters_filename, parameters)) return 0;

    std::vector<Antenna> antennas;


    if(!ReadAntennas(antennas, parameters)) return 0;

    Mesh mesh;
    const std::string stl_file_name = parameters["stl_file_name"].get<std::string>();

    if(!ReadTerrainMesh(mesh, stl_file_name)) return 0;

    if(!Compute(antennas, mesh)) return 0;

    PrintResultsInGidFormat(mesh, stl_file_name, TypeOfResultsPrint::RESULTS_ON_ELEMENTS);

    total_timer.stop();
    if(echo_level > 0) std::cout << "Total time: " << total_timer.getElapsedTimeInMilliSec() << "ms." << std::endl;
    if(echo_level > 0) std::cout << "Process finished normally."<<std::endl;
    return 0;
}
