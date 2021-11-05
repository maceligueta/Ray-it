#include "test.h"
#include <fstream>
#include <iostream>

bool Test::CheckMeshResultsAreEqualToReference(const std::string& results_mesh_file_name, const std::string& mesh_reference_file_name) {

        std::string version1, version2;

        std::ifstream results_mesh_file;
        results_mesh_file.open(results_mesh_file_name);
        if(results_mesh_file.fail()) {
            std::cout<<"Error opening file "<<results_mesh_file_name<< std::endl;
            return 1;
        }

        std::ifstream mesh_reference_file;
        mesh_reference_file.open(mesh_reference_file_name);
        if(mesh_reference_file.fail()) {
            std::cout<<"Error opening file "<<mesh_reference_file_name<< std::endl;
            return 1;
        }

        //Three lines of each file are headers
        if(!std::getline(results_mesh_file, version1)) return 1;
        if(!std::getline(results_mesh_file, version1)) return 1;
        if(!std::getline(results_mesh_file, version1)) return 1;
        std::getline(mesh_reference_file, version2);
        std::getline(mesh_reference_file, version2);
        std::getline(mesh_reference_file, version2);

        int problematic_line_number = -1;
        int line_count = 4;

        while(std::getline(mesh_reference_file, version2)) {
            std::getline(results_mesh_file, version1);
            if (version1 != version2) {
               problematic_line_number = line_count;
               break;
            }
            line_count++;
        }

        if (problematic_line_number>=0) {
            std::cout<<" Files are different at line "<<problematic_line_number<<std::endl;
            return 1;
        }


        results_mesh_file.close();
        mesh_reference_file.close();

        return 0;
    }