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
    mIdMapOfContributingBrdfs.resize(mNumberOfReflexions);
    mBrdfIndexForEachElement.resize(mMesh.mTriangles.size());

    for(int reflexion_number=0; reflexion_number<mNumberOfReflexions; reflexion_number++) {
        int count = 0;
        mBrdfIndexForEachElement[reflexion_number].resize(mMesh.mTriangles.size());
        for(size_t i=0; i<mMesh.mTriangles.size(); i++){
            if(mVectorsOfActiveElements[reflexion_number][i]){
                const Triangle& triangle = *mMesh.mTriangles[i];
                AntennaVariables empty_antenna_vars = AntennaVariables();
                empty_antenna_vars.mCoordinates = triangle.mCenter;
                empty_antenna_vars.mName = "";
                empty_antenna_vars.mVectorPointingFront = triangle.mNormal;
                empty_antenna_vars.mVectorPointingUp = triangle.mLocalAxis1;
                empty_antenna_vars.mRadiationPattern = std::make_shared<BRDFDiffuseRadiationPattern>(real_number(0.0), mAntennas[0].mRadiationPattern->mFrequency, real_number(20.0));
                Antenna empty_brdf = Antenna(empty_antenna_vars);
                mBrdfAntennas[reflexion_number].push_back(empty_brdf);
                mIdMapOfContributingBrdfs[reflexion_number].push_back(i);
                mBrdfIndexForEachElement[reflexion_number][i] = count;
                count++;
            }
            else{
                mBrdfIndexForEachElement[reflexion_number][i] = -1;
            }
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
        InitializeVectorsOfActiveElements();
        InitializeAllReflexionBrdfs();
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
            const real_number measuring_dist_squared = mAntennas[antenna_index].mRadiationPattern->mMeasuringDistance * mAntennas[antenna_index].mRadiationPattern->mMeasuringDistance;
            Vec3 vec_origin_to_triangle_center = Vec3(triangle.mCenter[0] - origin[0], triangle.mCenter[1] - origin[1], triangle.mCenter[2] - origin[2]);
            Ray ray(origin, vec_origin_to_triangle_center);
            ray.Intersect(mMesh);
            const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
            const real_number distance = std::sqrt(distance_squared);
            if(std::abs(ray.t_max - distance) < 1.0) {
                const JonesVector jones_vector_at_origin = mAntennas[antenna_index].GetDirectionalJonesVector(vec_origin_to_triangle_center);
                JonesVector jones_vector_at_destination = jones_vector_at_origin;
                jones_vector_at_destination.PropagateDistance(distance - mAntennas[antenna_index].mRadiationPattern->mMeasuringDistance);
                triangle.ProjectJonesVectorToTriangleAxesAndAdd(jones_vector_at_destination);

                if (mNumberOfReflexions) {
                    if(mVectorsOfActiveElements[0][i]){ //Only building and adding the brdf if this triangle will be used in the next reflexion
                        const real_number power_of_ray_received_by_triangle = jones_vector_at_destination.ComputeRMSPowerDensity() * triangle.ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle.mNormal);
                        #if RAY_IT_DEBUG
                        if(power_of_ray_received_by_triangle < 0.0) {
                            std::cout<<"Error: negative power!"<<std::endl;
                        }
                        #endif
                        const real_number power_of_ray_reflected_by_triangle = mFresnelReflexionCoefficient * mFresnelReflexionCoefficient * power_of_ray_received_by_triangle; //squared coefficient because we are reflecting power
                        if(power_of_ray_reflected_by_triangle * 0.25 * M_1_PI > mMinimumIntensityToBeReflected) {
                            Antenna brdf = BuildBrdfAtReflectionPoint(ray.mDirection, triangle, jones_vector_at_destination, power_of_ray_reflected_by_triangle);
                            int which_brdf_index = mBrdfIndexForEachElement[0][i];
                            mBrdfAntennas[0][which_brdf_index] += brdf;
                        }
                    }
                }
            }
        }
        triangle.mIntensity = triangle.ComputeRMSElectricFieldIntensityFromLocalAxesComponents();

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
    antenna_vars.mVectorPointingFront = triangle.mNormal;
    antenna_vars.mVectorPointingUp = triangle.mLocalAxis1;
    antenna_vars.mRadiationPattern = std::make_shared<BRDFDiffuseRadiationPattern>(power_of_ray_reflected_by_triangle, jones_vector_at_destination.mWaves[0].mFrequency, real_number(20.0));

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
            for(int i = 0; i<(int)mBrdfAntennas[reflexion_number].size(); i++) {
                vector_of_contributing_brdfs.push_back(&mBrdfAntennas[reflexion_number][i]);
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
                    if(contributor_brdf.mRadiationPattern->mTotalPower * 0.25 * M_1_PI < mMinimumIntensityToBeReflected) continue;
                    const size_t index_in_mesh = mIdMapOfContributingBrdfs[reflexion_number][j];

                    Vec3 vec_origin_to_triangle_center = Vec3(triangle.mCenter[0] - mMesh.mTriangles[index_in_mesh]->mCenter[0], triangle.mCenter[1] - mMesh.mTriangles[index_in_mesh]->mCenter[1], triangle.mCenter[2] - mMesh.mTriangles[index_in_mesh]->mCenter[2]);
                    if(Vec3::DotProduct(triangle.mNormal, vec_origin_to_triangle_center) > 0.0) continue; // It would mean that the ray comes from behind // OPTIONAL
                    if(Vec3::DotProduct(mMesh.mTriangles[index_in_mesh]->mNormal, vec_origin_to_triangle_center) < 0.0) continue; // It would mean that the ray goes through the floor of the brdf

                    Ray ray(mMesh.mTriangles[index_in_mesh]->mCenter, vec_origin_to_triangle_center);
                    ray.Intersect(mMesh);
                    const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                    const real_number distance = sqrt(distance_squared);

                    if(std::abs(ray.t_max - distance) < 1.0) {
                        const JonesVector jones_vector_at_origin = contributor_brdf.GetDirectionalJonesVector(vec_origin_to_triangle_center);
                        JonesVector jones_vector_at_destination = jones_vector_at_origin;
                        jones_vector_at_destination.PropagateDistance(distance - contributor_brdf.mRadiationPattern->mMeasuringDistance);
                        jones_vector_at_destination *= representation_factor;
                        triangle.ProjectJonesVectorToTriangleAxesAndAdd(jones_vector_at_destination);

                        if (mNumberOfReflexions > reflexion_number+1){
                            if(mVectorsOfActiveElements[reflexion_number+1][i]){ //Only building and adding the brdf if this triangle will be used in the next reflexion
                                const real_number power_of_ray_received_by_triangle = jones_vector_at_destination.ComputeRMSPowerDensity() * triangle.ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle.mNormal);
                                #if RAY_IT_DEBUG
                                if(power_of_ray_received_by_triangle < 0.0) {
                                    std::cout<<"Error: negative power!"<<std::endl;
                                }
                                #endif
                                const real_number power_of_ray_reflected_by_triangle = mFresnelReflexionCoefficient * mFresnelReflexionCoefficient * power_of_ray_received_by_triangle; //squared coefficient because we are reflecting power
                                if(power_of_ray_reflected_by_triangle * 0.25 * M_1_PI > mMinimumIntensityToBeReflected) {
                                    Antenna brdf = BuildBrdfAtReflectionPoint(ray.mDirection, triangle, jones_vector_at_destination, power_of_ray_reflected_by_triangle);
                                    int which_brdf_index = mBrdfIndexForEachElement[reflexion_number+1][i];
                                    mBrdfAntennas[reflexion_number+1][which_brdf_index] += brdf;
                                }
                            }
                        }
                    }
                }
                triangle.mIntensity = triangle.ComputeRMSElectricFieldIntensityFromLocalAxesComponents();

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