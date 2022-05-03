#ifndef __Ray_it__InputsReader__
#define __Ray_it__InputsReader__

#include <stdio.h>
#include <vector>
#include <sstream>
#include "omp.h"
#include "mesh.h"
#include "antenna.h"
#include "../external_libraries/stl_reader.h"
#include "../external_libraries/json.hpp"

using namespace nlohmann;
extern unsigned int RAY_IT_ECHO_LEVEL;

class InputsReader {
public:

    InputsReader(){};

    bool CheckPresenceOfKey(const json& input_parameters, const std::string key_to_be_checked) {
        if (!input_parameters.contains(key_to_be_checked)){
            std::cout<<"Error: the json input file must contain the key \""<<key_to_be_checked<<"\"";
            return false;
        }
        else{ return true; }
    }

    bool CheckInputParameters(json& input_parameters) {
        if(!CheckPresenceOfKey(input_parameters, "case_name")) return 1;
        if(!CheckPresenceOfKey(input_parameters, "terrain_input_settings")) return 1;
        if(!CheckPresenceOfKey(input_parameters,"antennas_list")) return 1;
        return 0;
    }

    bool ReadInputParameters(const std::string& parameters_filename, json& input_parameters) {

        std::ifstream i(parameters_filename);
        if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Reading input parameters file ("<<parameters_filename<<")..."<<std::endl;
        if (!std::filesystem::exists(parameters_filename)) {
            std::cout << "Error: file \""<<parameters_filename<<"\" not found!"<<std::endl;
            return 1;
        }
        try {
            i >> input_parameters;
        } catch (std::exception& e) {
            std::cout<<e.what()<<std::endl;
            return 1;
        }

        if(CheckInputParameters(input_parameters)) return 1;

        return 0;
    }

    std::string FindFolderOfFile(const std::string& file_name) {
        size_t pos = file_name.find_last_of("\\/");
        return (std::string::npos == pos) ? "" : file_name.substr(0, pos);
    }

    bool ReadTerrainMesh(Mesh& mesh, const json& terrain_input_settings) {

        const std::string& file_name = terrain_input_settings["file_name"].get<std::string>();

        if (terrain_input_settings["type"].get<std::string>() == "stl"){
            ReadTerrainSTLMesh(mesh, file_name);
            return 0;
        }
        if (terrain_input_settings["type"].get<std::string>() == "asc"){
            if(terrain_input_settings["keep_one_node_out_of"].is_number_integer()){
                const int keep_one_node_out_of = terrain_input_settings["keep_one_node_out_of"].get<int>();
                if(keep_one_node_out_of>=1){
                    ReadTerrainASCMesh(mesh, file_name, keep_one_node_out_of);
                    return 0;
                }
                else {
                    std::cout<<"ERROR: the value 'keep_one_node_out_of' must be greater or equal than 1"<<std::endl;
                    return 1;
                }
            }
            else {
                std::cout<<"ERROR: the value 'keep_one_node_out_of' must be an integer"<<std::endl;
                return 1;
            }
        }
        return 0;
    }

    void PrintError(const std::string& file_name, const int line_count) {
        std::cout<<"Error reading file "<<file_name<<" at line number "<<line_count<<std::endl;
    }

    bool ReadTerrainASCMesh(Mesh& mesh, const std::string& file_name, const int& keep_one_node_out_of) {

        //std::cout<<std::filesystem::current_path()<<std::endl;
        std::string file_name_to_be_used_here = file_name;

        if (!std::filesystem::exists(file_name)) {
            const std::string file_name_with_current_path = CURRENT_WORKING_DIR + "/" + file_name;
            if (!std::filesystem::exists(file_name_with_current_path)) {
                std::cout << "Error: files \""<<file_name<<"\" or \""<<file_name_with_current_path<<"\" not found!"<<std::endl;
                return 1;
            }
            else{
               file_name_to_be_used_here = file_name_with_current_path;
            }
        }

        std::ifstream asc_file;
        std::string line;
        std::stringstream line_stream; // include <sstream>
        std::string token;
        int line_count = 0;
        asc_file.open(file_name_to_be_used_here);

        getline(asc_file, line); line_count++;
        line_stream = std::stringstream(line);
        line_stream >> token;
        if(token != "NCOLS") { PrintError(file_name_to_be_used_here, line_count); return 1;}
        line_stream >> token;
        const int ncols = std::stoi(token);

        getline(asc_file, line); line_count++;
        line_stream = std::stringstream(line);
        line_stream >> token;
        if(token != "NROWS") { PrintError(file_name_to_be_used_here, line_count); return 1;}
        line_stream >> token;
        const int nrows = std::stoi(token);

        getline(asc_file, line); line_count++;
        line_stream = std::stringstream(line);
        line_stream >> token;
        if(token != "XLLCORNER") { PrintError(file_name_to_be_used_here, line_count); return 1;}
        line_stream >> token;
        const real_number xllcorner = real_number(std::stod(token));

        getline(asc_file, line); line_count++;
        line_stream = std::stringstream(line);
        line_stream >> token;
        if(token != "YLLCORNER") { PrintError(file_name_to_be_used_here, line_count); return 1;}
        line_stream >> token;
        const real_number yllcorner = real_number(std::stod(token));

        getline(asc_file, line); line_count++;
        line_stream = std::stringstream(line);
        line_stream >> token;
        if(token != "CELLSIZE") { PrintError(file_name_to_be_used_here, line_count); return 1;}
        line_stream >> token;
        const real_number cellsize = real_number(std::stod(token));

        getline(asc_file, line); line_count++;
        line_stream = std::stringstream(line);
        line_stream >> token;
        if(token != "NODATA_VALUE") { PrintError(file_name_to_be_used_here, line_count); return 1;}
        line_stream >> token;
        const int nodata_value = std::stoi(token);


        real_number xmin = INFINITY, ymin = INFINITY, zmin = INFINITY;
        real_number xmax = -INFINITY, ymax = -INFINITY, zmax = -INFINITY;

        mesh.mNodes.reserve(nrows*ncols);

        int actual_rows_read = 0;
        int actual_cols_read = 0;
        int counter = 0;
        for (int i=0; i<nrows; i++){
            getline(asc_file, line); line_count++;
            line_stream = std::stringstream(line);
            actual_cols_read = 0;
            for(int j=0; j<ncols; j++){
                line_stream >> token;
                real_number x = xllcorner + j * cellsize;
                real_number y = yllcorner + (nrows - i - 1) * cellsize;
                real_number z = real_number(std::stod(token));
                Vec3 node(x, y, z);
                mesh.mNodes.push_back(Node(counter, node));
                counter++;
                xmin = fmin(xmin, node[0]);
                ymin = fmin(ymin, node[1]);
                zmin = fmin(zmin, node[2]);
                xmax = fmax(xmax, node[0]);
                ymax = fmax(ymax, node[1]);
                zmax = fmax(zmax, node[2]);
                actual_cols_read++;
                for(int l=0; l<keep_one_node_out_of-1; l++){
                    line_stream >> token;
                    j++;
                }
            }
            actual_rows_read++;
            for(int k=0; k<keep_one_node_out_of-1; k++){
                getline(asc_file, line); line_count++;
                i++;
            }
        }

        mesh.mRowsOfStructuredMeshNodes = actual_rows_read;
        mesh.mColumnsOfStructuredMeshNodes = actual_cols_read;

        real_number added_tolerance = 0.001f * fmax( xmax-xmin, fmax(ymax-ymin, zmax-zmin));
        mesh.mBoundingBox[0] = Vec3(xmin-added_tolerance, ymin-added_tolerance, zmin-added_tolerance);
        mesh.mBoundingBox[1] = Vec3(xmax+added_tolerance, ymax+added_tolerance, zmax+added_tolerance);

        std::vector<std::vector<Triangle*>> triangles_by_threads;
        triangles_by_threads.resize(omp_get_max_threads());
        for (int i=0; i<omp_get_max_threads(); i++){
            triangles_by_threads[i].reserve( nrows * ncols * 2 / omp_get_max_threads());
        }
        #pragma omp parallel for
        for (int i=0; i<actual_rows_read-1; i++){
            for(int j=0; j<actual_cols_read-1; j++){
                Triangle* t = new Triangle(mesh.mNodes[j + i * actual_cols_read],
                                            mesh.mNodes[j + (i+1) * actual_cols_read],
                                            mesh.mNodes[(j+1) + i * actual_cols_read]);
                triangles_by_threads[omp_get_thread_num()].push_back(t);

                Triangle* t2 = new Triangle(mesh.mNodes[(j+1) + i * actual_cols_read],
                                            mesh.mNodes[j + (i+1) * actual_cols_read],
                                            mesh.mNodes[(j+1) + (i+1) * actual_cols_read]);

                triangles_by_threads[omp_get_thread_num()].push_back(t2);
            }
        }

        for (int i=0; i<omp_get_max_threads(); i++){
            mesh.mTriangles.insert( mesh.mTriangles.end(), triangles_by_threads[i].begin(), triangles_by_threads[i].end() );
        }


        if (RAY_IT_ECHO_LEVEL > 0) std::cout<<"File "<<file_name_to_be_used_here<<" was read correctly. "<<mesh.mNodes.size()<<" nodes and "<<mesh.mTriangles.size()<<" elements."<<std::endl;

        return 0;

    }

    bool ReadTerrainSTLMesh(Mesh& mesh, const std::string& file_name) {

        if (!std::filesystem::exists(file_name)) {
            std::cout << "Error: file \""<<file_name<<"\" not found!"<<std::endl;
            return 1;
        }

        try {

            if (RAY_IT_ECHO_LEVEL > 0) std::cout<<"Reading STL mesh ("<<file_name<<")..."<<std::endl;
            stl_reader::StlMesh <real_number, unsigned int> stl_mesh (file_name);

            if (RAY_IT_ECHO_LEVEL > 1) {
                for(size_t itri = 0; itri < stl_mesh.num_tris(); ++itri) {
                    std::cout << "Coords " << itri << ": ";
                    for(size_t icorner = 0; icorner < 3; ++icorner) {
                        const real_number* c = stl_mesh.tri_corner_coords(itri, icorner);
                        std::cout << "(" << c[0] << ", " << c[1] << ", " << c[2] << ") ";
                    }
                    std::cout << std::endl;

                    std::cout << "Indices " << itri << ": ( ";
                    for(size_t icorner = 0; icorner < 3; ++icorner) {
                        const unsigned int c =  stl_mesh.tri_corner_ind (itri, icorner);
                        std::cout << c << " ";
                    }
                    std::cout << ")" << std::endl;

                    const real_number* n = stl_mesh.tri_normal(itri);
                    std::cout << "normal of triangle " << itri << ": "
                            << "(" << n[0] << ", " << n[1] << ", " << n[2] << ")\n";
                }
            }

            real_number xmin = INFINITY, ymin = INFINITY, zmin = INFINITY;
            real_number xmax = -INFINITY, ymax = -INFINITY, zmax = -INFINITY;

            for(int i=0; i<stl_mesh.num_vrts(); i++){
                Vec3 node(stl_mesh.vrt_coords(i)[0], stl_mesh.vrt_coords(i)[1], stl_mesh.vrt_coords(i)[2]);
                mesh.mNodes.push_back(Node(i, node));
                xmin = fmin(xmin, node[0]);
                ymin = fmin(ymin, node[1]);
                zmin = fmin(zmin, node[2]);
                xmax = fmax(xmax, node[0]);
                ymax = fmax(ymax, node[1]);
                zmax = fmax(zmax, node[2]);
            }

            if (RAY_IT_ECHO_LEVEL > 1) {
                std::cout << "List of nodes and coordinates: "<<std::endl;
                for(size_t i = 0; i < mesh.mNodes.size(); i++) {
                    std::cout<<i<<"  "<<mesh.mNodes[i][0]<<"  "<<mesh.mNodes[i][1]<<"  "<<mesh.mNodes[i][2]<<std::endl;
                }
            }

            real_number added_tolerance = 0.001f * fmax( xmax-xmin, fmax(ymax-ymin, zmax-zmin));
            mesh.mBoundingBox[0] = Vec3(xmin-added_tolerance, ymin-added_tolerance, zmin-added_tolerance);
            mesh.mBoundingBox[1] = Vec3(xmax+added_tolerance, ymax+added_tolerance, zmax+added_tolerance);

            for(int i = 0; i < stl_mesh.num_tris(); i++){
                //Vec3 N(stl_mesh.tri_normal(i)[0], stl_mesh.tri_normal(i)[1], stl_mesh.tri_normal(i)[2]);
                //mesh.mNormals.push_back(N);

                Triangle* t = new Triangle(mesh.mNodes[stl_mesh.tri_corner_ind(i, 0)],
                                            mesh.mNodes[stl_mesh.tri_corner_ind(i, 1)],
                                            mesh.mNodes[stl_mesh.tri_corner_ind(i, 2)]);

                mesh.mTriangles.push_back(t);
            }

            if (RAY_IT_ECHO_LEVEL > 0) std::cout<<"File "<<file_name<<" was read correctly. "<<mesh.mNodes.size()<<" nodes and "<<mesh.mTriangles.size()<<" elements."<<std::endl;

        } catch (std::exception& e) {
            std::cout<<e.what()<<std::endl;
            return 1;
        }

        return 0;
    }

    bool ReadAntennas(std::vector<Antenna>& antennas, const json& input_parameters){

        for (auto& single_antenna_data : input_parameters["antennas_list"]) {
            Antenna a = Antenna(single_antenna_data);
            antennas.push_back(a);
        }
        return 0;
    }

};
#endif