#include <stdio.h>
#include <vector>
#include <filesystem>
#include <omp.h>

#include "constants.h"
#include "time.h"
#include "vector.h"
#include "mesh.h"
#include "ray.h"
#include "kdTree.h"
#include "tests_manager.h"
#include "computation.h"
#include "../external_libraries/timer.h"
#include "../external_libraries/json.hpp"

using namespace nlohmann;

int main(int argc, char *argv[]) {

    int test_number = 0;

    if(argc!=2){
        if(argc == 3) {
            try {
                test_number = std::stoi(argv[2]);
            } catch(std::invalid_argument const& ex) {
                std::cout << "Error trying to extract the number of test from third argument ("<< ex.what() << ")\n";
                return 1;
            }
            if ( test_number == 0) {
                std::cout << "Error trying to extract the number of test from third argument \n";
                return 1;
            }
        } else {
            std::cout<<"Error: wrong number of arguments. Type the argument 'tests' to run the tests, or type the paramters file informing about the input data (antennas and terrain) and settings."<<std::endl;
            return 1;
        }
    }

    if(RAY_IT_ECHO_LEVEL > 0) {
        std::cout << "Ray-it starting. Using ";
        #ifdef RAY_IT_USE_FLOATS
        #pragma message("---- Compiling with single precision ----")
        std::cout << "single precision ('float' variables). ";
        #else
        #pragma message("---- Compiling with double precision ----")
        std::cout << "double precision ('double' variables). ";
        #endif
        std::cout<<std::endl<<std::endl;

        #ifdef RAY_IT_DEBUG
                    #pragma message("---- Compiling in Debug mode ----")
        #else
            #pragma message("---- Compiling in Release mode ----")
        #endif
    }

    Timer total_timer;
    total_timer.start();

    std::string stl_filename;
    std::string parameters_filename;

    if(strcmp(argv[1], "tests") == 0) {
        if(test_number == 0) {
            if (RunAllTests()) return 1;
        }
        else {
            RunTest(test_number);
        }
    } else {
        parameters_filename = argv[1];

        json parameters;
        InputsReader reader;
        CURRENT_WORKING_DIR = reader.FindFolderOfFile(parameters_filename);
        if(reader.ReadInputParameters(parameters_filename, parameters)) return 1;

        Computation job;
        if( job.Run(parameters) ) return 1;
    }

    total_timer.stop();
    std::cout << "\nTotal time: " << total_timer.getElapsedTimeInSec() << " s." << std::endl;
    std::cout << "Process finished normally."<<std::endl;
    return 0;
}
