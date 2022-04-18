#ifndef __Ray_ittest14
#define __Ray_ittest14

#include "test.h"
#include "../src/radiation_pattern.h"
#include "../src/computation.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>

class ComputationForTest14: public Computation {
    bool ReadAntennas(std::vector<Antenna>& antennas, const json& parameters) override {
        if( ! Computation::ReadAntennas(antennas, parameters) ) {
            for(auto& ant:antennas){
                auto& rm = ant.mRadiationPattern;
                rm.mFrequency = 1.0e6; // 1 MHz
                rm.mMeasuringDistance = 2.0;
                auto& map = rm.mRadiationMap;
                rm.mTotalPower = 1000.0;
                for(size_t i=0; i<map.size(); i++) {
                    for(size_t j=0; j<map[i].size(); j++) {
                        map[i][j][EPhi] = 0.0;
                        map[i][j][ETheta] = 10.0;
                        map[i][j][EPhiPhase] = 0.0;
                        map[i][j][EThetaPhase] = 0.0;
                    }
                }
            }
            return 0;
        } else {
            return 1;
        }
    }
};

class Test14: public Test {

    public:

    Test14():Test(){
        mNumber = 14;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber<<"... ";

        RAY_IT_ECHO_LEVEL = 0;

        json parameters;

        std::string parameters_filename  = "cases/double_antenna.json";
        InputsReader reader;
        CURRENT_WORKING_DIR = reader.FindFolderOfFile(parameters_filename);
        if(reader.ReadInputParameters(parameters_filename, parameters)) return 1;

        ComputationForTest14 job;
        if( job.Run(parameters) ) return 1;

        #ifdef RAY_IT_USE_FLOATS
        std::string reference_result_file_name = "cases/reference14_float.post.res";
        #else
        std::string reference_result_file_name = "cases/reference14_double.post.res";
        #endif

        return CheckMeshResultsAreEqualToReference("cases/double_antenna_over_1km_square.post.res", reference_result_file_name);

        return 0;
    }

};

#endif