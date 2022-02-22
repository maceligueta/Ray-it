#ifndef __Ray_ittest13
#define __Ray_ittest13

#include "test.h"
#include "../src/ray-it.h"
#include "../src/radiation_pattern.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>

class Test13: public Test {

    public:

    Test13():Test(){
        mNumber = 13;
    }
    bool Run() override{
        std::cout<<"Running test "<<mNumber<<"... ";

        RAY_IT_ECHO_LEVEL = 0;


        json single_antenna_data = json::parse(R"({
            "name": "Test13 antenna",
            "coordinates":[0.0, 0.0, 0.0],
            "power": 1.0,
            "orientation":{
                "front": [1.0, 0.0, 0.0],
                "up": [0.0, 0.0, 1.0]
            },
            "radiation_pattern_file_name": "Dipole1.out"
        })");

        Antenna a;
        if(a.InitializeFromParameters(single_antenna_data)){
            return 1;
        }

        return 0;
    }

};

#endif /* defined(__Ray_ittest13) */