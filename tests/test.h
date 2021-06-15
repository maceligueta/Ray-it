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

        char version1, version2;
        int count = 0, k = 0;
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

        results_mesh_file.get(version1);
        mesh_reference_file.get(version2);
        while(!results_mesh_file.eof() && !mesh_reference_file.eof()) {
            if (version1 != version2) k++;
            results_mesh_file.get(version1);
            mesh_reference_file.get(version2);
        }

        if (k!=0) {
            std::cout<<" Files are different "<<std::endl;
            return false;
        }


        results_mesh_file.close();
        mesh_reference_file.close();

        return true;
    }
};

#endif