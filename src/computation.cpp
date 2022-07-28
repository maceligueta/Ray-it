#include "computation.h"

#include "../external_libraries/MeshPlaneIntersect.hpp"
#include "bullington.h"
#include <stdlib.h>

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

void Computation::FillNextArrayWithEmptyBrdfs(const int which_vector) {
    mBrdfAntennas[which_vector].clear();
    mIdMapOfContributingBrdfs[which_vector].clear();
    int count = 0;
    for(size_t i=0; i<mMesh.mTriangles.size(); i++){
        if(RandomBoolAccordingToProbabilityFast(mPortionOfElementsContributingToReflexion)) {
            const Triangle& triangle = *mMesh.mTriangles[i];
            AntennaVariables empty_antenna_vars = AntennaVariables();
            empty_antenna_vars.mCoordinates = triangle.mCenter;
            empty_antenna_vars.mName = "";
            empty_antenna_vars.mVectorPointingFront = triangle.mNormal;
            empty_antenna_vars.mVectorPointingUp = triangle.mLocalAxis1;
            empty_antenna_vars.mRadiationPattern = std::make_shared<BRDFDiffuseRadiationPattern>(real_number(0.0), mAntennas[0].mRadiationPattern->mFrequency, real_number(20.0));
            Antenna empty_brdf = Antenna(empty_antenna_vars);
            mBrdfAntennas[which_vector].push_back(empty_brdf);
            mIdMapOfContributingBrdfs[which_vector].push_back(i);
            mBrdfIndexForEachElement[which_vector][i] = count;
            count++;
        }
        else{
            mBrdfIndexForEachElement[which_vector][i] = -1;
        }
    }
    mNumberOfActiveElements[which_vector] = count;
}

void Computation::InitializeAllReflexionBrdfs() {
    mBrdfAntennas.resize(2);
    mIdMapOfContributingBrdfs.resize(2);
    mBrdfIndexForEachElement.resize(2);
    mNumberOfActiveElements.resize(2);
    for(int r=0; r<2; r++) {
        mBrdfIndexForEachElement[r].resize(mMesh.mTriangles.size());
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
    }
    mDiffractionModel = computation_settings["diffraction_model"].get<std::string>();

    return 0;
}

void Computation::ComputeDirectIncidence() {
    if(RAY_IT_ECHO_LEVEL > 0) std::cout << "\nComputation starts. Computing direct incidence... "<<std::endl;

    if (mNumberOfReflexions) {
        FillNextArrayWithEmptyBrdfs(0);
    }

    progressbar bar((int)mMesh.mTriangles.size());
    bar.set_done_char("*");

    #pragma omp parallel
    {
    int thread_iteration_counter = 0;
    const size_t jump_between_progress_bar_update = mMesh.mTriangles.size() / 100;

    #pragma omp for schedule(dynamic, 50)
    for(int i = 0; i<(int)mMesh.mTriangles.size(); i++) {
        Triangle& triangle = *mMesh.mTriangles[i];
        for(size_t antenna_index=0; antenna_index<mAntennas.size(); ++antenna_index) {
            Vec3 origin = mAntennas[antenna_index].mCoordinates;
            Vec3 vec_origin_to_triangle_center = Vec3(triangle.mCenter[0] - origin[0], triangle.mCenter[1] - origin[1], triangle.mCenter[2] - origin[2]);
            Ray ray(origin, vec_origin_to_triangle_center);
            ray.Intersect(mMesh);
            const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
            const real_number distance = std::sqrt(distance_squared);
            if(ray.mIdOfFirstCrossedTriangle == triangle.mId) {
                const JonesVector jones_vector_at_origin = mAntennas[antenna_index].GetDirectionalJonesVector(vec_origin_to_triangle_center);
                JonesVector jones_vector_at_destination = jones_vector_at_origin;
                jones_vector_at_destination.PropagateDistance(distance - mAntennas[antenna_index].mRadiationPattern->mMeasuringDistance);
                triangle.ProjectJonesVectorToTriangleAxesAndAdd(jones_vector_at_destination);

                if (mNumberOfReflexions) {
                    int which_brdf_index = mBrdfIndexForEachElement[0][i];
                    if(which_brdf_index >= 0){ //Only building and adding the brdf if this triangle will be used in the next reflexion
                        const real_number power_of_ray_received_by_triangle = jones_vector_at_destination.ComputeRMSPowerDensity() * triangle.ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle.mNormal);
                        #if RAY_IT_DEBUG
                        if(power_of_ray_received_by_triangle < 0.0) {
                            std::cout<<"Error: negative power!"<<std::endl;
                        }
                        #endif
                        const real_number power_of_ray_reflected_by_triangle = mFresnelReflexionCoefficient * mFresnelReflexionCoefficient * power_of_ray_received_by_triangle; //squared coefficient because we are reflecting power
                        Antenna brdf_to_be_added = BuildBrdfAtReflectionPoint(ray.mDirection, triangle, jones_vector_at_destination, power_of_ray_reflected_by_triangle);

                        auto& this_triangle_brdf = mBrdfAntennas[0][which_brdf_index];
                        this_triangle_brdf += brdf_to_be_added;
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
            const int index_for_current_reflexion = reflexion_number % 2;
            const int index_for_next_reflexion = (reflexion_number + 1) % 2;
            if (mNumberOfReflexions > reflexion_number+1){
                FillNextArrayWithEmptyBrdfs(index_for_next_reflexion);
            }
            if(RAY_IT_ECHO_LEVEL > 0) std::cout << "\n\nComputing reflexion "<< reflexion_number + 1 << " of "<<mNumberOfReflexions<<" with "<<mNumberOfActiveElements[index_for_current_reflexion]<<" rays to each triangle..."<<std::endl;

            progressbar bar((int)mMesh.mTriangles.size());
            bar.set_done_char("*");
            //bar.show_bar(false);
            #pragma omp parallel
            {
            int thread_iteration_counter = 0;
            const size_t jump_between_progress_bar_update = mMesh.mTriangles.size() / 100;

            #pragma omp for schedule(dynamic, 50)
            for(int i = 0; i<(int)mMesh.mTriangles.size(); i++) {
                Triangle& triangle = *mMesh.mTriangles[i];
                for(int j = 0; j<(int)mBrdfAntennas[index_for_current_reflexion].size(); j++) {
                    const Antenna& contributor_brdf = mBrdfAntennas[index_for_current_reflexion][j];
                    const size_t index_of_emitting_triangle = mIdMapOfContributingBrdfs[index_for_current_reflexion][j];
                    if(index_of_emitting_triangle == i) continue;
                    if(contributor_brdf.mRadiationPattern->mTotalPower * 0.25 * M_1_PI < mMinimumIntensityToBeReflected) continue;

                    const auto& emitting_triangle = mMesh.mTriangles[index_of_emitting_triangle];

                    Vec3 vec_origin_to_triangle_center = Vec3(triangle.mCenter[0] - emitting_triangle->mCenter[0], triangle.mCenter[1] - emitting_triangle->mCenter[1], triangle.mCenter[2] - emitting_triangle->mCenter[2]);
                    if(Vec3::DotProduct(triangle.mNormal, vec_origin_to_triangle_center) > EPSILON) continue; // It would mean that the ray comes from behind or parallel // OPTIONAL
                    if(Vec3::DotProduct(emitting_triangle->mNormal, vec_origin_to_triangle_center) < EPSILON) continue; // It would mean that the ray goes through the floor of the brdf or comes parallel

                    Ray ray(emitting_triangle->mCenter, vec_origin_to_triangle_center);
                    ray.Intersect(mMesh);
                    const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                    const real_number distance = sqrt(distance_squared);

                    if(ray.mIdOfFirstCrossedTriangle == triangle.mId) {
                        const JonesVector jones_vector_at_origin = contributor_brdf.GetDirectionalJonesVector(vec_origin_to_triangle_center);
                        JonesVector jones_vector_at_destination = jones_vector_at_origin;
                        jones_vector_at_destination.PropagateDistance(distance - contributor_brdf.mRadiationPattern->mMeasuringDistance);
                        jones_vector_at_destination *= representation_factor;
                        triangle.ProjectJonesVectorToTriangleAxesAndAdd(jones_vector_at_destination);

                        if (mNumberOfReflexions > reflexion_number+1){
                            if(mBrdfIndexForEachElement[index_for_next_reflexion][i] >= 0){ //Only building and adding the brdf if this triangle will be used in the next reflexion
                                const real_number power_of_ray_received_by_triangle = jones_vector_at_destination.ComputeRMSPowerDensity() * triangle.ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle.mNormal);
                                #if RAY_IT_DEBUG
                                if(power_of_ray_received_by_triangle < 0.0) {
                                    std::cout<<"Error: negative power!"<<std::endl;
                                }
                                #endif
                                const real_number power_of_ray_reflected_by_triangle = mFresnelReflexionCoefficient * mFresnelReflexionCoefficient * power_of_ray_received_by_triangle; //squared coefficient because we are reflecting power
                                Antenna brdf_to_be_added = BuildBrdfAtReflectionPoint(ray.mDirection, triangle, jones_vector_at_destination, power_of_ray_reflected_by_triangle);
                                int which_brdf_index = mBrdfIndexForEachElement[index_for_next_reflexion][i];
                                auto& this_triangle_brdf = mBrdfAntennas[index_for_next_reflexion][which_brdf_index];
                                this_triangle_brdf += brdf_to_be_added;
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

void Computation::ComputeDiffraction() {
    if(mDiffractionModel != "Bullington") return;

    if(RAY_IT_ECHO_LEVEL > 0) std::cout << "\nComputing effect of diffraction ("<<mDiffractionModel<<")..."<<std::endl;
    typedef MeshPlaneIntersect<real_number, int> Intersector;
    std::vector<Intersector::Vec3D> vertices;
    for(size_t i = 0; i<mMesh.mNodes.size(); i++) {
        vertices.push_back(Intersector::Vec3D{ {mMesh.mNodes[i][0], mMesh.mNodes[i][1], mMesh.mNodes[i][2]} });
    }
    std::vector<Intersector::Face> faces;
    for(size_t i = 0; i<mMesh.mTriangles.size(); i++) {
        const int& a = mMesh.mTriangles[i]->mNodeIndices[0];
        const int& b = mMesh.mTriangles[i]->mNodeIndices[1];
        const int& c = mMesh.mTriangles[i]->mNodeIndices[2];
        faces.push_back(Intersector::Face{ {a, b, c} });
    }

    Intersector::Mesh intersectable_mesh(vertices, faces);

    progressbar bar((int)mMesh.mTriangles.size());
    bar.set_done_char("*");

    #pragma omp parallel
    {
    int thread_iteration_counter = 0;
    const size_t jump_between_progress_bar_update = mMesh.mTriangles.size() / 100;

    #pragma omp for schedule(dynamic, 50)
    for(int i = 0; i<(int)mMesh.mTriangles.size(); i++) {
        Triangle& triangle = *mMesh.mTriangles[i];
        for(size_t antenna_index=0; antenna_index<mAntennas.size(); ++antenna_index) {
            const auto& origin = mAntennas[antenna_index].mCoordinates;
            Vec3 vec_origin_to_triangle_center = Vec3(triangle.mCenter[0] - origin[0], triangle.mCenter[1] - origin[1], triangle.mCenter[2] - origin[2]);
            Vec3 horizontal_dir = Vec3(vec_origin_to_triangle_center[0], vec_origin_to_triangle_center[1], 0.0);
            Intersector::Plane plane;
            plane.origin = {{origin[0], origin[1], origin[2]}};
            Vec3 normal = Vec3::CrossProduct(vec_origin_to_triangle_center, Vec3(0, 0, 1));
            plane.normal = {{normal[0], normal[1], normal[2]}};
            const auto& result = intersectable_mesh.Intersect(plane);

            std::vector<Vec3> profile;
            profile.push_back(origin);
            for(int k=0; k<result.size(); k++){
                for(int j=0; j<result[k].points.size(); j++) {
                    const auto& result_coords = result[k].points[j];
                    Vec3 origin_to_profile_point(result_coords[0] - origin[0], result_coords[1] - origin[1], result_coords[2] - origin[2]);
                    Vec3 target_to_profile_point(result_coords[0] - triangle.mCenter[0], result_coords[1] - triangle.mCenter[1], result_coords[2] - triangle.mCenter[2]);
                    if(Vec3::DotProduct(horizontal_dir, origin_to_profile_point)>0.0 && Vec3::DotProduct(horizontal_dir, target_to_profile_point)<0.0) {
                        profile.push_back(Vec3(result_coords[0],result_coords[1], result_coords[2]));

                    }
                }
            }
            profile.push_back(triangle.mCenter);
            std::sort(profile.begin(), profile.end(), [origin](const Vec3& lhs, const Vec3& rhs){ return Vec3SquareDistance(origin, lhs) < Vec3SquareDistance(origin, rhs); });
            //OutputsWriter().PrintProfileInXYZFormat(profile, "profile_"+std::to_string(i));

            std::vector<real_number> heights;
            std::vector<real_number> distances;
            for(int k=0; k<profile.size(); k++){
                heights.push_back(profile[k][2]);
                const real_number horizontal_squared_distance = (profile[k][0]-origin[0])*(profile[k][0]-origin[0]) + (profile[k][1]-origin[1])*(profile[k][1]-origin[1]);
                distances.push_back(sqrt(horizontal_squared_distance));
            }

            #if RAY_IT_DEBUG
            for(int l=0; l<distances.size()-1; l++){
                if(distances[l+1]<distances[l]){
                    std::cout<<"\nWARNNIG: profile distances not in ascending order! \n";
                    break;
                }
            }
            #endif

            real_number db_loss_wrt_free_space = 0.0;
            real_number slope_from_transmitter = 0.0;
            BullingtonDiffraction::ComputeBullington(distances, heights, mAntennas[antenna_index].mRadiationPattern->mFrequency, db_loss_wrt_free_space, slope_from_transmitter);

            VecC3 electric_field_before_diffraction;

            if(triangle.mIntensity<EPSILON) {
                const real_number distance_squared = vec_origin_to_triangle_center[0] * vec_origin_to_triangle_center[0] + vec_origin_to_triangle_center[1] *vec_origin_to_triangle_center[1] + vec_origin_to_triangle_center[2] * vec_origin_to_triangle_center[2];
                const real_number distance = std::sqrt(distance_squared); // TODO: QUESTIONABLE, the distance travelled by the wave is not the direct line. However, Bullington's estimation is far from correct either.
                const Vec3 transmitter_to_edge_direction = horizontal_dir.Normalize() + Vec3(0.0, 0.0, slope_from_transmitter);
                const JonesVector jones_vector_at_origin = mAntennas[antenna_index].GetDirectionalJonesVector(transmitter_to_edge_direction);
                JonesVector jones_vector_at_destination = jones_vector_at_origin;
                jones_vector_at_destination.PropagateDistance(distance - mAntennas[antenna_index].mRadiationPattern->mMeasuringDistance);
                electric_field_before_diffraction = triangle.ProjectJonesVectorToTriangleAxes(jones_vector_at_destination);
            } else {
                electric_field_before_diffraction = triangle.mElectricFieldWithoutDiffraction;
            }

            const VecC3 electric_field_after_diffraction = electric_field_before_diffraction * std::pow(real_number(10.0), -db_loss_wrt_free_space * real_number(0.05));
            triangle.mElectricFieldDueToDiffraction = electric_field_after_diffraction - triangle.mElectricFieldWithoutDiffraction;
            triangle.mIntensity = triangle.ComputeRMSElectricFieldIntensityFromLocalAxesComponents();
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

bool Computation::ComputeRays(const json& computation_settings){

    InitializeComputationOfRays(computation_settings);
    ComputeDirectIncidence();
    ComputeDiffraction();
    ComputeEffectOfReflexions();

    if(RAY_IT_ECHO_LEVEL > 0) std::cout << "\n\nComputation finished."<<std::endl;

    return 0;
}