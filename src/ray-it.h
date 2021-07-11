#ifndef RayTracer_main
#define RayTracer_main

#include "constants.h"
#include "mesh.h"
#include "../external_libraries/json.hpp"
#include "antenna.h"

bool CheckInputParameters(nlohmann::json& input_parameters);

bool ReadTerrainMesh(Mesh& mesh, const std::string& filename);
bool ReadAntennas(std::vector<Antenna>& antennas, nlohmann::json& input_parameters);
bool Compute(const std::vector<Antenna>& antennas, Mesh& mesh);
void PrintResultsInGidFormat(Mesh& mesh, const std::string& file_name, const TypeOfResultsPrint& print_type);

#endif