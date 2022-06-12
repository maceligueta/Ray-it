#ifndef __Ray_it__Computation__
#define __Ray_it__Computation__

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include "antenna.h"
#include "constants.h"
#include "inputs_reader.h"
#include "outputs_writer.h"
#include "wave.h"
#include "jones.h"
#include "mesh.h"
#include "../external_libraries/json.hpp"
#include <random>
#include "geometric_operations.h"
#include "radiation_patterns/custom_radiation_patterns.h"
#include "../external_libraries/progressbar.hpp"

using namespace nlohmann;

class Computation {

public:

    Computation(){}

    Mesh mMesh;
    std::vector<Antenna> mAntennas;
    std::knuth_b mRandomEngine;
    std::vector<int> mNumberOfActiveElements;
    int mNumberOfReflexions;
    std::vector<std::vector<Antenna>> mBrdfAntennas;
    real_number mFresnelReflexionCoefficient;
    real_number mMinimumIntensityToBeReflected;
    real_number mPortionOfElementsContributingToReflexion;
    std::vector<std::vector<bool>> mVectorsOfActiveElements;


    virtual bool Run(const json& parameters);

    virtual bool ReadAntennas(const json& parameters) {
        InputsReader reader;
        if(reader.ReadAntennas(mAntennas, parameters)) return 1;
        else return 0;
    }

    virtual bool ReadTerrainMesh(const json& parameters) {
        InputsReader reader;
        if(reader.ReadTerrainMesh(mMesh, parameters)) return 1;
        else return 0;
    }

    virtual bool RandomBoolAccordingToProbabilityFast(const double p){
        return rand() < p * (RAND_MAX+1.0);
    }

    virtual void InitializeVectorsOfActiveElements();

    virtual bool RandomBoolAccordingToProbability(std::uniform_real_distribution<>& uniform_distribution_zero_to_one, const real_number prob) {  // probability between 0.0 and 1.0
        return uniform_distribution_zero_to_one(mRandomEngine) < prob;
    }

    virtual void InitializeAllReflexionBrdfs();
    virtual bool InitializeComputationOfRays(const json& computation_settings);
    virtual void ComputeDirectIncidence();
    virtual bool ComputeRays(const json& computation_settings);
    virtual bool BuildKdTree();
    virtual bool PrintResults(const json& parameters);
    virtual void ComputeEffectOfReflexions();
    Antenna BuildBrdfAtReflectionPoint(const Vec3& ray_direction, const Triangle& triangle, const JonesVector& jones_vector_at_destination, const real_number& power_of_ray_reflected_by_triangle);

};
#endif