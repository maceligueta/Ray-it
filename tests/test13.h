#ifndef __Ray_ittest13
#define __Ray_ittest13

#include "test.h"
#include "../src/radiation_pattern.h"
#include "../src/constants.h"
#include "../src/radiation_patterns/brdf_diffuse_radiation_pattern.h"

class Test13: public Test {

    public:

    Test13():Test(){
        mNumber = 13;
    }
    bool Run() override{
        std::cout<<"Running test "<<std::setw(3)<<std::setfill('0')<<mNumber <<"... ";

        /*json single_antenna_data = json::parse(R"({
            "name": "Test13 antenna",
            "coordinates":[0.0, 0.0, 0.0],
            "power": 1.0,
            "orientation":{
                "front": [1.0, 0.0, 0.0],
                "up": [0.0, 0.0, 1.0]
            },
            "radiation_pattern_file_name": "cases/Dipole1.out"
        })");

        Antenna a;
        if(a.InitializeFromParameters(single_antenna_data)){
            return 1;
        }*/

        Vec3 origin = Vec3(0.0, 0.0, 3.0);

        Node n0 = Node(0, 0.0, 0.0, 0.0);
        Node n1 = Node(1, 1.0, 0.0, 0.0);
        Node n2 = Node(2, 0.0, 1.0, 0.0);

        Triangle triangle = Triangle(n0, n1, n2);

        const Vec3 vec_origin_to_triangle_center = triangle.mCenter - origin;
        Vec3 unitary_vec_origin_to_triangle_center = vec_origin_to_triangle_center.Normalize();
        Ray ray(origin, vec_origin_to_triangle_center);

        Vec3 horizontal_normal_to_ray = Vec3(real_number(-1.0)* vec_origin_to_triangle_center[1]/vec_origin_to_triangle_center[0], 1.0, 0.0);
        Vec3 phi_dir = -1.0 * horizontal_normal_to_ray.Normalize();
        Vec3 theta_dir = Vec3::CrossProduct(unitary_vec_origin_to_triangle_center, horizontal_normal_to_ray);

        // Propagation of one ray:
        const real_number E_vertical = 10.0;
        const real_number E_horizontal = 5.0;
        const real_number frequency = 3.0e7;

        JonesVector jones_vector_at_origin = JonesVector(Wave(3.0e7, E_vertical, 0.0, 1.0),
                                                        Wave(3.0e7, E_horizontal, 0.0, 1.0),
                                                        theta_dir,
                                                        phi_dir);
        const real_number power_density_at_origin = (E_vertical*E_vertical + E_horizontal*E_horizontal) * INVERSE_OF_IMPEDANCE_OF_FREE_SPACE;
        const real_number jones_power_density_at_origin = jones_vector_at_origin.ComputePowerDensity();
        if(!CheckIfValuesAreEqual(power_density_at_origin, jones_power_density_at_origin)) return 1;

        const real_number power_density_at_destination = power_density_at_origin * real_number(1.0) / (std::pow(vec_origin_to_triangle_center.Norm(), 2));

        JonesVector jones_vector_at_destination = jones_vector_at_origin;
        jones_vector_at_destination.PropagateDistance(vec_origin_to_triangle_center.Norm() - real_number(1.0));

        const real_number jones_power_density = jones_vector_at_destination.ComputePowerDensity();
        if(!CheckIfValuesAreEqual(power_density_at_destination, jones_power_density)) return 1;

        const real_number jones_intensity = jones_vector_at_destination.ComputeElectricFieldIntensity();
        if(!CheckIfValuesAreEqual(jones_intensity*jones_intensity* INVERSE_OF_IMPEDANCE_OF_FREE_SPACE, power_density_at_destination)) return 1;

        const real_number total_power_received_by_triangle = jones_intensity*jones_intensity * INVERSE_OF_IMPEDANCE_OF_FREE_SPACE * triangle.ComputeArea() * Vec3::DotProduct(ray.mDirection * -1.0, triangle.mNormal);

        //propagation of another ray:

        // creating empty brdf to add up contributions from different rays
        AntennaVariables empty_antenna_vars = AntennaVariables();
        empty_antenna_vars.mCoordinates = triangle.mCenter;
        empty_antenna_vars.mName = "";
        empty_antenna_vars.mVectorPointingFront = triangle.mNormal;
        empty_antenna_vars.mVectorPointingUp = (triangle.mP0 - triangle.mCenter).Normalize();
        empty_antenna_vars.mRadiationPattern = BRDFDiffuseRadiationPattern(0.0, frequency, 10.0);
        Antenna added_up_brdf = Antenna(empty_antenna_vars);

        //Compute main reflection direction
        Vec3 reflection_dir = GeometricOperations::ComputeReflectionDirection(ray.mDirection, triangle.mNormal);

        //Check the properties expected from main reflection direction
        const real_number incident_projected_to_normal = Vec3::DotProduct(ray.mDirection, triangle.mNormal);
        const real_number reflected_projected_to_normal = Vec3::DotProduct(-1.0*reflection_dir, triangle.mNormal);
        if(!CheckIfValuesAreEqual(incident_projected_to_normal, reflected_projected_to_normal)) return 1;

        //Compute normal of triangle from incident and reflected directions and check
        Vec3 estimated_normal = GeometricOperations::ComputeNormalFromIncidentAndReflected(ray.mDirection, reflection_dir);
        if(!CheckIfValuesAreEqual(estimated_normal, triangle.mNormal)) return 1;


        // Build a BRDF at the reflection point (triangle center)
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
        antenna_vars.mRadiationPattern = BRDFDiffuseRadiationPattern(total_power_received_by_triangle, frequency, 10.0); // Here we are reflecting all the power received (not realistic)

        Antenna brdf1 = Antenna(antenna_vars);
        brdf1.FillReflectedPatternInfoFromIncidentRay(unitary_vec_origin_to_triangle_center, OrientedJonesVector(jones_vector_at_destination), triangle.mNormal);

        added_up_brdf += brdf1;

        Node n3 = Node(3, 0.0, 0.0, 6.0);
        Node n4 = Node(4, 1.0, 0.0, 6.0);
        Node n5 = Node(5, 0.0, 1.0, 6.0);

        Triangle triangle2 = Triangle(n3, n4, n5);

        const Vec3 vec_triangle1_center_to_triangle2_center = triangle2.mCenter - triangle.mCenter;
        Vec3 unitary_vec_triangle1_center_to_triangle2_center = vec_triangle1_center_to_triangle2_center.Normalize();
        Ray ray2(origin, unitary_vec_triangle1_center_to_triangle2_center);

        JonesVector second_jones_vector = added_up_brdf.GetDirectionalJonesVector(unitary_vec_triangle1_center_to_triangle2_center);
        JonesVector second_jones_vector_for_check = brdf1.GetDirectionalJonesVector(unitary_vec_triangle1_center_to_triangle2_center);
        //here we check the PowerDensity, it makes no sense to compare amplitudes because those amplitudes are related to the orientation of the
        //tangent vectors, which change with the orientation of the antenna:
        if(!CheckIfValuesAreEqual(second_jones_vector.ComputePowerDensity(), second_jones_vector_for_check.ComputePowerDensity())) return 1;

        OrientedJonesVector a = OrientedJonesVector(second_jones_vector);
        OrientedJonesVector b = OrientedJonesVector(second_jones_vector_for_check);
        if(!CheckIfValuesAreEqual(a, b)) return 1;


        JonesVector second_jones_vector_at_destination = second_jones_vector;
        second_jones_vector_at_destination.PropagateDistance(vec_triangle1_center_to_triangle2_center.Norm() - real_number(1.0));


        return 0;
    }

};

#endif