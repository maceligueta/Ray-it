#include "computation.h"

extern unsigned int RAY_IT_ECHO_LEVEL;

bool Computation::Run(const json& parameters) {
    if(ReadAntennas(parameters)) return 1;
    if(ReadTerrainMesh(parameters["terrain_input_settings"])) return 1;
    if(BuildKdTree()) return 1;
    if(ComputeRays(parameters["computation_settings"])) return 1;
    if(PrintResults(parameters)) return 1;
    return 0;
}

bool Computation::BuildKdTree() {
    if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Building KD-tree... " << std::endl;
    KDTreeNode* root= new KDTreeNode();
    mMesh.mTree = *root->RecursiveTreeNodeBuild(mMesh.mTriangles, Box(mMesh.mBoundingBox[0], mMesh.mBoundingBox[1]), 0, SplitPlane());
    if(RAY_IT_ECHO_LEVEL > 0) std::cout << "Finished building KD-tree."<< std::endl;
    return 0;
}

bool Computation::PrintResults(const json& parameters) {
    OutputsWriter writer;
    const std::string output_file_name_with_current_path = CURRENT_WORKING_DIR + "/" + parameters["case_name"].get<std::string>();
    if(parameters["output_settings"]["print_for_gid"].get<bool>()) {
        writer.PrintResultsInGidFormat(mMesh, mAntennas, output_file_name_with_current_path, TypeOfResultsPrint::RESULTS_ON_ELEMENTS);
    }
    if(parameters["output_settings"]["print_for_matlab"].get<bool>()) {
        writer.PrintResultsInMatlabFormat(mMesh, mAntennas, TypeOfResultsPrint::RESULTS_ON_ELEMENTS);
    }
    return 0;
}

void Computation::InitializeAllReflexionBrdfs() {
    mBrdfAntennas.resize(mNumberOfReflexions);
    for(int reflexion_number=0; reflexion_number<mNumberOfReflexions; reflexion_number++) {
        mBrdfAntennas[reflexion_number].resize(mMesh.mTriangles.size());
        //TODO: parallelize this?
        for(size_t i=0; i<mMesh.mTriangles.size(); i++){
            const Triangle& triangle = *mMesh.mTriangles[i];
            AntennaVariables empty_antenna_vars = AntennaVariables();
            empty_antenna_vars.mCoordinates = triangle.mCenter;
            empty_antenna_vars.mName = "";
            empty_antenna_vars.mVectorPointingFront = triangle.mNormal;
            empty_antenna_vars.mVectorPointingUp = (triangle.mP0 - triangle.mCenter).Normalize();
            empty_antenna_vars.mRadiationPattern = BRDFDiffuseRadiationPattern(0.0, mAntennas[0].mRadiationPattern.mFrequency, 10.0);
            Antenna empty_brdf = Antenna(empty_antenna_vars);
            mBrdfAntennas[reflexion_number][i] = empty_brdf;
        }

    }
}

void Computation::InitializeVectorsOfActiveElements() {
    mVectorsOfActiveElements.resize(mNumberOfReflexions);
    mNumberOfActiveElements.resize(mNumberOfReflexions);

    for(size_t i=0; i<mVectorsOfActiveElements.size(); i++){
        int count = 0;
        mVectorsOfActiveElements[i].resize(mMesh.mTriangles.size());
        for(size_t j=0; j<mVectorsOfActiveElements[i].size(); j++) {
            mVectorsOfActiveElements[i][j] = RandomBoolAccordingToProbabilityFast(mPortionOfElementsContributingToReflexion);
            if(mVectorsOfActiveElements[i][j]) count++;
        }
        mNumberOfActiveElements[i] = count;
    }
}

bool Computation::InitializeComputationOfRays(const json& computation_settings) {
    if(RAY_IT_ECHO_LEVEL > 0) std::cout << "\nInitializing computation... "<<std::endl;
    mNumberOfReflexions = computation_settings["number_of_reflexions"].get<int>();

    const json montecarlo_settings = computation_settings["montecarlo_settings"];

    if(montecarlo_settings["type_of_decimation"].get<std::string>() == "portion_of_elements") {
        mPortionOfElementsContributingToReflexion = real_number(montecarlo_settings["portion_of_elements_contributing_to_reflexion"].get<double>());
    } else if (montecarlo_settings["type_of_decimation"].get<std::string>() == "number_of_rays") {
        mPortionOfElementsContributingToReflexion = real_number(montecarlo_settings["number_of_rays"].get<int>()) / mMesh.mTriangles.size();
        if(mPortionOfElementsContributingToReflexion > 1.0) mPortionOfElementsContributingToReflexion = real_number(1.0);
    }
    mFresnelReflexionCoefficient =  real_number(computation_settings["Fresnel_reflexion_coefficient"].get<double>());
    mMinimumIntensityToBeReflected = real_number(computation_settings["minimum_intensity_to_be_reflected"].get<double>());

    if(mNumberOfReflexions) {
        InitializeAllReflexionBrdfs();
        InitializeVectorsOfActiveElements();
    }
    return 0;
}

void Computation::ComputeDirectIncidence() {
    if(RAY_IT_ECHO_LEVEL > 0) std::cout << "\nComputation starts. Computing direct incidence... "<<std::endl;

    progressbar bar((int)mMesh.mTriangles.size());
    bar.set_done_char("*");

    #pragma omp parallel
    {
    int thread_iteration_counter = 0;
    const size_t jump_between_progress_bar_update = mMesh.mTriangles.size() / 100;

    #pragma omp for schedule(dynamic, 500)
    for(int i = 0; i<(int)mMesh.mTriangles.size(); i++) {
        Triangle& triangle = *mMesh.mTriangles[i];
        for(size_t antenna_index=0; antenna_index<mAntennas.size(); ++antenna_index) {
            Vec3 origin = mAntennas[antenna_index].mCoordinates;
            const real_number measuring_dist_squared = mAntennas[antenna_index].mRadiationPattern.mMeasuringDistance * mAntennas[antenna_index].mRadiationPattern.mMeasuringDistance;
            Vec3 vec_origin_to_triangle_center = Vec3(triangle.mCenter[0] - origin[0], triangle.mCenter[1] - origin[1], triangle.mCenter[2] - origin[2]);
            Ray ray(origin, vec_origin_to_triangle_center);
            ray.Intersect(mMesh);
            const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
            const real_number distance = std::sqrt(distance_squared);
            if(std::abs(ray.t_max - distance) < 1.0) {
                const JonesVector jones_vector_at_origin = mAntennas[antenna_index].GetDirectionalJonesVector(vec_origin_to_triangle_center);
                JonesVector jones_vector_at_destination = jones_vector_at_origin;
                jones_vector_at_destination.PropagateDistance(distance - mAntennas[antenna_index].mRadiationPattern.mMeasuringDistance);
                triangle.ProjectJonesVectorToTriangleAxesAndAdd(jones_vector_at_destination);
                triangle.mIntensity = triangle.ComputeRMSElectricFieldIntensityFromLocalAxesComponents();

                if (mNumberOfReflexions) {
                    const real_number power_of_ray_received_by_triangle = jones_vector_at_destination.ComputeRMSPowerDensity() * triangle.ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle.mNormal);
                    #if RAY_IT_DEBUG
                    if(power_of_ray_received_by_triangle < 0.0) {
                        std::cout<<"Error: negative power!"<<std::endl;
                    }
                    #endif
                    const real_number power_of_ray_reflected_by_triangle = mFresnelReflexionCoefficient * mFresnelReflexionCoefficient * power_of_ray_received_by_triangle; //squared coefficient because we are reflecting power
                    Antenna brdf = BuildBrdfAtReflectionPoint(ray.mDirection, triangle, jones_vector_at_destination, power_of_ray_reflected_by_triangle);
                    mBrdfAntennas[0][i] += brdf;
                }
            }
        }
        thread_iteration_counter++;
        if(thread_iteration_counter == jump_between_progress_bar_update && RAY_IT_ECHO_LEVEL>0) {
            thread_iteration_counter = 0;
            #pragma omp critical
            {
            bar.update((int)jump_between_progress_bar_update);
            }
        }
    }
    }
}

Antenna Computation::BuildBrdfAtReflectionPoint(const Vec3& ray_direction, const Triangle& triangle, const JonesVector& jones_vector_at_destination, const real_number& power_of_ray_reflected_by_triangle) {

    Vec3 reflection_dir = GeometricOperations::ComputeReflectionDirection(ray_direction, triangle.mNormal);
    AntennaVariables antenna_vars = AntennaVariables();
    antenna_vars.mCoordinates = triangle.mCenter;
    antenna_vars.mName = "";
    antenna_vars.mVectorPointingFront = reflection_dir;

    if(Vec3::DotProduct(reflection_dir, triangle.mNormal) < 1.0-EPSILON) {
        antenna_vars.mVectorPointingUp = Vec3::CrossProduct(reflection_dir, Vec3::CrossProduct(triangle.mNormal, reflection_dir));
    }
    else {
        antenna_vars.mVectorPointingUp = triangle.mLocalAxis1;
    }
    antenna_vars.mRadiationPattern = BRDFDiffuseRadiationPattern(power_of_ray_reflected_by_triangle, jones_vector_at_destination.mWaves[0].mFrequency, 10.0);

    Antenna brdf = Antenna(antenna_vars);
    brdf.FillReflectedPatternInfoFromIncidentRay(ray_direction, OrientedJonesVector(jones_vector_at_destination), triangle.mNormal);

    return brdf;
}

void Computation::ComputeEffectOfReflexions() {
    if(mNumberOfReflexions){
        std::uniform_real_distribution<> uniform_distribution_zero_to_one(0.0, 1.0);

        const real_number representation_factor = real_number(1.0) / mPortionOfElementsContributingToReflexion;

        for(int reflexion_number=0; reflexion_number<mNumberOfReflexions; reflexion_number++) {
            if(RAY_IT_ECHO_LEVEL > 0) std::cout << "\n\nComputing reflexion "<< reflexion_number + 1 << " with "<<mNumberOfActiveElements[reflexion_number]<<" rays to each triangle..."<<std::endl;

            std::vector<Antenna*> vector_of_contributing_brdfs;
            std::vector<int> id_map_of_contributing_brdfs;
            for(int i = 0; i<(int)mBrdfAntennas[reflexion_number].size(); i++) {
                if( mVectorsOfActiveElements[reflexion_number][i]) {
                    vector_of_contributing_brdfs.push_back(&mBrdfAntennas[reflexion_number][i]);
                    id_map_of_contributing_brdfs.push_back(i);
                }
            }
            progressbar bar((int)mMesh.mTriangles.size());
            bar.set_done_char("*");
            //bar.show_bar(false);
            #pragma omp parallel
            {
            int thread_iteration_counter = 0;
            const size_t jump_between_progress_bar_update = mMesh.mTriangles.size() / 100;

            #pragma omp for schedule(dynamic, 500)
            for(int i = 0; i<(int)mMesh.mTriangles.size(); i++) {
                Triangle& triangle = *mMesh.mTriangles[i];
                for(int j = 0; j<(int)vector_of_contributing_brdfs.size(); j++) {
                    const Antenna& contributor_brdf = *vector_of_contributing_brdfs[j];
                    if(j == i) continue;
                    if(contributor_brdf.mRadiationPattern.mTotalPower * 0.25 * M_1_PI < mMinimumIntensityToBeReflected) continue;
                    const int index_in_mesh = id_map_of_contributing_brdfs[j];
                    if(Vec3::DotProduct(triangle.mNormal, mMesh.mTriangles[index_in_mesh]->mNormal) < 0.0) continue;

                    Vec3 vec_origin_to_triangle_center = Vec3(triangle.mCenter[0] - mMesh.mTriangles[index_in_mesh]->mCenter[0], triangle.mCenter[1] - mMesh.mTriangles[index_in_mesh]->mCenter[1], triangle.mCenter[2] - mMesh.mTriangles[index_in_mesh]->mCenter[2]);
                    Ray ray(mMesh.mTriangles[index_in_mesh]->mCenter, vec_origin_to_triangle_center);
                    ray.Intersect(mMesh);
                    const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                    const real_number distance = sqrt(distance_squared);

                    if(std::abs(ray.t_max - distance) < 1.0) {
                        const JonesVector jones_vector_at_origin = contributor_brdf.GetDirectionalJonesVector(vec_origin_to_triangle_center);
                        JonesVector jones_vector_at_destination = jones_vector_at_origin;
                        jones_vector_at_destination.PropagateDistance(distance - contributor_brdf.mRadiationPattern.mMeasuringDistance);
                        jones_vector_at_destination *= representation_factor;
                        triangle.ProjectJonesVectorToTriangleAxesAndAdd(jones_vector_at_destination);
                        triangle.mIntensity = triangle.ComputeRMSElectricFieldIntensityFromLocalAxesComponents();

                        if (mNumberOfReflexions > reflexion_number+1){
                            const real_number power_of_ray_received_by_triangle = jones_vector_at_destination.ComputeRMSPowerDensity() * triangle.ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle.mNormal);
                            #if RAY_IT_DEBUG
                            if(power_of_ray_received_by_triangle < 0.0) {
                                std::cout<<"Error: negative power!"<<std::endl;
                            }
                            #endif
                            const real_number power_of_ray_reflected_by_triangle = mFresnelReflexionCoefficient * mFresnelReflexionCoefficient * power_of_ray_received_by_triangle; //squared coefficient because we are reflecting power
                            Antenna brdf = BuildBrdfAtReflectionPoint(ray.mDirection, triangle, jones_vector_at_destination, power_of_ray_reflected_by_triangle);
                            mBrdfAntennas[reflexion_number+1][i] += brdf;
                        }
                    }
                }
                thread_iteration_counter++;
                if(thread_iteration_counter == jump_between_progress_bar_update && RAY_IT_ECHO_LEVEL>0) {
                    thread_iteration_counter = 0;
                    #pragma omp critical
                    {
                    bar.update((int)jump_between_progress_bar_update);
                    }
                }
            }
            }
        }
    }
}

bool Computation::ComputeRays(const json& computation_settings){

    InitializeComputationOfRays(computation_settings);
    ComputeDirectIncidence();
    ComputeEffectOfReflexions();

    if(RAY_IT_ECHO_LEVEL > 0) std::cout << "\n\nComputation finished."<<std::endl;

    return 0;
}