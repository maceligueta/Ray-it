#ifndef __Ray_it__DxfParser__
#define __Ray_it__DxfParser__

#include <stdio.h>
#include <vector>
#include <sstream>
#include "omp.h"
#include "mesh.h"
#include "../external_libraries/json.hpp"

using namespace nlohmann;
extern unsigned int RAY_IT_ECHO_LEVEL;

static bool AddDxfInfoToMesh(Mesh& mesh, const std::string& dxf_filename) {
    real_number xmin = INFINITY, ymin = INFINITY, zmin = INFINITY;
    real_number xmax = -INFINITY, ymax = -INFINITY, zmax = -INFINITY;

    for(size_t i=0; i<mesh.mNodes.size(); i++) {
        const auto& coords = mesh.mNodes[i].mCoordinates;
        if(coords[0] > xmax) { xmax = coords[0];}
        if(coords[0] < xmin) { xmin = coords[0];}
        if(coords[1] > ymax) { ymax = coords[1];}
        if(coords[1] < ymin) { ymin = coords[1];}
        if(coords[2] > zmax) { zmax = coords[2];}
        if(coords[2] < zmin) { zmin = coords[2];}
    }

    size_t initial_global_node_count = mesh.mNodes.size();
    size_t initial_global_elem_count = mesh.mTriangles.size();
    size_t global_node_count = mesh.mNodes.size();
    size_t global_elem_count = mesh.mTriangles.size();
    size_t num_polyline_entities = 0;

    std::ifstream file_reader(dxf_filename);
    std::string line;
    std::stringstream line_stream; // include <sstream>
    std::string token;
    int line_count = 0;

    while (getline(file_reader, line)) {
        line_count++;
        line_stream = std::stringstream(line);
        line_stream >> token;
        if(token == "ENTITIES"){
            while(getline(file_reader, line)) {
                line_count++;
                line_stream = std::stringstream(line);
                line_stream >> token;
                if(token == "ENDSEC") {
                    break;
                }
                if(token == "POLYLINE") {
                    num_polyline_entities++;
                    size_t local_node_count = 0;
                    size_t local_face_count = 0;
                    size_t num_vertices = 0;
                    size_t num_faces = 0;
                    while(getline(file_reader, line)) {
                        line_count++;
                        line_stream = std::stringstream(line);
                        line_stream >> token;
                        if(token == "SEQEND") {
                            break;
                        }
                        if(token == "71"){
                            getline(file_reader, line); line_count++;
                            line_stream = std::stringstream(line);
                            line_stream >> token;
                            num_vertices = std::stoi(token);
                            getline(file_reader, line); line_count++;
                            getline(file_reader, line); line_count++;
                            line_stream = std::stringstream(line);
                            line_stream >> token;
                            num_faces = std::stoi(token);
                            getline(file_reader, line); line_count++;
                            for(int i=0; i<num_vertices; i++){
                                while(getline(file_reader, line)) {
                                    line_count++;
                                    line_stream = std::stringstream(line);
                                    line_stream >> token;
                                    if(token == "10"){
                                        getline(file_reader, line); line_count++;
                                        line_stream = std::stringstream(line);
                                        line_stream >> token;
                                        const double coordsX = std::stod(token);
                                        getline(file_reader, line); line_count++;
                                        getline(file_reader, line); line_count++;
                                        line_stream = std::stringstream(line);
                                        line_stream >> token;
                                        const double coordsY = std::stod(token);
                                        getline(file_reader, line); line_count++;
                                        getline(file_reader, line); line_count++;
                                        line_stream = std::stringstream(line);
                                        line_stream >> token;
                                        const double coordsZ = std::stod(token);
                                        mesh.mNodes.push_back(Node(int(local_node_count+global_node_count), coordsX, coordsY, coordsZ));
                                        if(coordsX > xmax) { xmax = coordsX;}
                                        if(coordsX < xmin) { xmin = coordsX;}
                                        if(coordsY > ymax) { ymax = coordsY;}
                                        if(coordsY < ymin) { ymin = coordsY;}
                                        if(coordsZ > zmax) { zmax = coordsZ;}
                                        if(coordsZ < zmin) { zmin = coordsZ;}
                                        local_node_count++;
                                        break;
                                    }
                                }
                            }
                            for(int i=0; i<num_faces; i++){
                                while(getline(file_reader, line)) {
                                    line_count++;
                                    line_stream = std::stringstream(line);
                                    line_stream >> token;
                                    if(token == "71"){
                                        getline(file_reader, line); line_count++;
                                        line_stream = std::stringstream(line);
                                        line_stream >> token;
                                        const int vertex_index0 = std::stoi(token);
                                        getline(file_reader, line); line_count++;
                                        getline(file_reader, line); line_count++;
                                        line_stream = std::stringstream(line);
                                        line_stream >> token;
                                        const int vertex_index1 = std::stoi(token);
                                        getline(file_reader, line); line_count++;
                                        getline(file_reader, line); line_count++;
                                        line_stream = std::stringstream(line);
                                        line_stream >> token;
                                        const int vertex_index2 = std::stoi(token);
                                        mesh.mTriangles.push_back(new Triangle(int(local_face_count+global_elem_count),
                                                                                mesh.mNodes[int(vertex_index0 - 1 + global_node_count)],
                                                                                mesh.mNodes[int(vertex_index1 - 1 + global_node_count)],
                                                                                mesh.mNodes[int(vertex_index2 - 1 + global_node_count)]));
                                        local_face_count++;
                                        break;
                                    }
                                }
                            }
                            global_node_count += num_vertices;
                            global_elem_count += num_faces;
                        }
                    }
                }
            }

        }
    }

    if(RAY_IT_ECHO_LEVEL > 0) {
    //std::cout<<"Number of polylines read: "<<num_polyline_entities<<"\n";
    //std::cout<<"Number of nodes added: "<<global_node_count-initial_global_node_count<<"\n";
    //std::cout<<"Number of triangles added: "<<global_elem_count-initial_global_elem_count<<"\n";
    }

    real_number added_tolerance = 0.001f * fmax( xmax-xmin, fmax(ymax-ymin, zmax-zmin));
    mesh.mBoundingBox[0] = Vec3(xmin-added_tolerance, ymin-added_tolerance, zmin-added_tolerance);
    mesh.mBoundingBox[1] = Vec3(xmax+added_tolerance, ymax+added_tolerance, zmax+added_tolerance);

    return 0;
}

#endif