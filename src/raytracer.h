#ifndef RayTracer_main
#define RayTracer_main

#include "mesh.h"

bool ReadTerrainMesh(Mesh& mesh, const std::string& filename);
void PrintResultsInGidFormat(Mesh& mesh, const std::string& file_name, const TypeOfResultsPrint& print_type);

#endif