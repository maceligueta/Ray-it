#ifndef RayTracer_test
#define RayTracer_test

#include <limits>
#include "../src/constants.h"

class Test {

    public:

    Test(){};
    unsigned int mNumber = 0;
    virtual bool Run(){
        return 1;
    }
    bool CheckIfFloatsAreEqual(const float& a, const float& b) {
        auto c = std::numeric_limits<float>::epsilon();
        return (std::abs(a - b) < c);
    }

    bool CheckMeshResultsAreEqualToReference(const std::string& results_mesh_file_name, const std::string& mesh_reference_file_name) {

        std::string version1, version2;

        std::ifstream results_mesh_file;
        results_mesh_file.open(results_mesh_file_name);
        if(results_mesh_file.fail()) {
            std::cout<<"Error opening file "<<results_mesh_file_name<< std::endl;
            return false;
        }

        std::ifstream mesh_reference_file;
        mesh_reference_file.open(mesh_reference_file_name);
        if(mesh_reference_file.fail()) {
            std::cout<<"Error opening file "<<mesh_reference_file_name<< std::endl;
            return false;
        }

        //Three lines of each file are headers
        if(!std::getline(results_mesh_file, version1)) return false;
        if(!std::getline(results_mesh_file, version1)) return false;
        if(!std::getline(results_mesh_file, version1)) return false;
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
            return false;
        }


        results_mesh_file.close();
        mesh_reference_file.close();

        return true;
    }
};

#endif