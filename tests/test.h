#ifndef RayTracer_test
#define RayTracer_test

#include <limits>
#include <string>
#include <iostream>
#include "../src/constants.h"

class Test {

    public:

    Test(){};
    unsigned int mNumber = 0;
    virtual bool Run(){
        return 1;
    }
    bool CheckIfValuesAreEqual(const real_number& a, const real_number& b) {
        if(std::abs(a - b) < EPSILON){
            return true;
        }
        else {
            std::cout<<a<<" is not equal to "<<b<<std::endl;
            return false;
        }
    }

    bool CheckMeshResultsAreEqualToReference(const std::string& results_mesh_file_name, const std::string& mesh_reference_file_name);
};

#endif