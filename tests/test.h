#ifndef RayTracer_test
#define RayTracer_test

#include <limits>
#include <string>
#include "../src/constants.h"

class Test {

    public:

    Test(){};
    unsigned int mNumber = 0;
    virtual bool Run(){
        return 1;
    }
    bool CheckIfFloatsAreEqual(const real& a, const real& b) {
        auto c = std::numeric_limits<real>::epsilon();
        return (std::abs(a - b) < c);
    }

    bool CheckMeshResultsAreEqualToReference(const std::string& results_mesh_file_name, const std::string& mesh_reference_file_name);
};

#endif