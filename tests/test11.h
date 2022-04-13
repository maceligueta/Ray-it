#ifndef __Ray_ittest11
#define __Ray_ittest11

#include "test.h"
#include "../src/radiation_pattern.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>

class Test11: public Test {

    public:

    Test11():Test(){
        mNumber = 11;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        RAY_IT_ECHO_LEVEL = 0;

        json parameters;

        std::string parameters_filename  = "cases/simple_run_matlab_output.json";
        InputsReader reader;
        CURRENT_WORKING_DIR = reader.FindFolderOfFile(parameters_filename);
        if(reader.ReadInputParameters(parameters_filename, parameters)) return 1;

        Computation job;
        if( job.Run(parameters) ) return 1;

        return 0;
    }

};

#endif