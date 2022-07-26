#include <stdio.h>
#include <vector>
#include <omp.h>
#include <chrono>

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
extern unsigned int RAY_IT_ECHO_LEVEL;

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
        std::cout << "\n -- Ray-it starting --\nUsing ";
        #ifdef RAY_IT_USE_FLOATS
        #pragma message("---- Compiling with single precision ----")
        std::cout << "single precision ('float' variables). ";
        #else
        #pragma message("---- Compiling with double precision ----")
        std::cout << "double precision ('double' variables). ";
        #endif
        std::cout<<std::endl;

        #ifdef RAY_IT_DEBUG
                    #pragma message("---- Compiling in Debug mode ----")
        #else
            #pragma message("---- Compiling in Release mode ----")
        #endif

        std::cout<<"Number of processors: "<<omp_get_max_threads()<<std::endl;
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
        total_timer.stop();
        std::cout << "\nTotal time: " << total_timer.getElapsedTimeInSec() << " s." << std::endl;
        std::cout << "Process finished normally."<<std::endl;
    } else {

        parameters_filename = argv[1];

        json parameters;
        InputsReader reader;
        CURRENT_WORKING_DIR = reader.FindFolderOfFile(parameters_filename);


        if(reader.ReadInputParameters(parameters_filename, parameters)) return 1;


        #pragma warning(disable : 4996)
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_c);
        char time_string [80];
        strftime (time_string, 80, "%F@%H.%M.%S", &now_tm);
        const std::string log_out_filename = CURRENT_WORKING_DIR + "/" + "run_log_"+ time_string + ".out";
        std::ofstream log_out(log_out_filename);
        log_out<<"--- Log file for Ray-it --- \n\n";
        strftime (time_string, 80, "%F_%T", &now_tm);
        log_out<<"Computation started: "<<time_string<<"\n\n";
        log_out<<"With these input parameters: \n";
        log_out<<parameters.dump(4)<<"\n"<<std::flush;


        Computation job;
        if( job.Run(parameters) ) return 1;


        total_timer.stop();
        std::cout << "\nTotal time: " << total_timer.getElapsedTimeInSec() << " s.\n";
        std::cout << "Process finished normally and log was printed in file "<<log_out_filename<<std::endl;
        log_out<<"\nComputation ended: "<<time_string<<"\n";
        log_out << "\nTotal time: " << total_timer.getElapsedTimeInSec() << " s." << std::endl;
    }


    return 0;
}
