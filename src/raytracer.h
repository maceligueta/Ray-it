#ifndef RayTracer_main
#define RayTracer_main

#include "mesh.h"
#include "../external_libraries/json.hpp"

bool CheckInputParameters(nlohmann::json& input_parameters);
bool ReadTerrainMesh(Mesh& mesh, const std::string& filename);
bool Compute(Mesh& mesh);
void PrintResultsInGidFormat(Mesh& mesh, const std::string& file_name, const TypeOfResultsPrint& print_type);

#endif