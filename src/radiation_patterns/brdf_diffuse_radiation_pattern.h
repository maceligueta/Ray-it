#ifndef __Ray_it_brdf_diffuse_radiation_pattern__
#define __Ray_it_brdf_diffuse_radiation_pattern__

#include "../radiation_pattern.h"

class BRDFDiffuseRadiationPattern: public RadiationPattern {
    public:

    BRDFDiffuseRadiationPattern(const real_number total_power, const real_number frequency, const real_number angle_between_values){

        mTotalPower = total_power;
        mFrequency = frequency;
        mMeasuringDistance = 1.0;

        mSeparationBetweenPhiValues = angle_between_values;
        mSeparationBetweenThetaValues = angle_between_values;
        mRadiationMap.resize(int((GetTopLimitPhi()-GetBottomLimitPhi()) / mSeparationBetweenPhiValues) + 1); //ONLY HALF THE SPHERE!!
        for(size_t i=0;i<mRadiationMap.size(); i++) {
            mRadiationMap[i].resize(int(180.0 / mSeparationBetweenThetaValues) + 1);
            for (size_t j=0; j<mRadiationMap[i].size(); j++) {
                const SphericalCoordinates sc = GetSphericalCoordinatesFromIndices(i, j);
                if(sc.mPhi < -90 || sc.mPhi > 90) {
                    mRadiationMap[i][j][Gain] = real_number(-1e100);
                }
                else {
                    mRadiationMap[i][j][Gain] = real_number(3.0274657687865787);
                }
            }
        }
        //ScaleDirectionalGainForConsistentIntegral();


    };

    real_number IntegratePatternSurfaceTotalPowerBasedOnGain() override {
        #include "../points_on_unit_sphere.h"
        const auto& p = POINT_COORDINATES_ON_SPHERE;
        const size_t num_points = p.size();
        const real_number weight_of_each_point = real_number(4.0 * M_PI / num_points);
        const real_number isotropic_power_density = mTotalPower / real_number(4.0 * M_PI * mMeasuringDistance * mMeasuringDistance);
        real_number integral = 0.0;
        for(int i=0; i<num_points; i++){
            Vec3 dir = Vec3(p[i][0], p[i][1], p[i][2]);
            if(p[i][0]<0.0) continue;
            const auto sp = SphericalCoordinates(dir);
            /*if(sp.mPhi < GetBottomLimitPhi() ||  sp.mPhi > GetTopLimitPhi() || sp.mTheta < GetBottomLimitTheta() || sp.mTheta > GetTopLimitTheta()) {
                continue;
            }*/
            const real_number gain = GetDirectionalGainValue(sp);
            const real_number power_density = isotropic_power_density * std::pow(real_number(10.0), gain * real_number(0.1));
            integral += power_density;
        }
        integral *= weight_of_each_point;

        return integral;
    }

    real_number IntegratePatternSurfaceTotalPowerBasedOnElectricField() override {
        #include "../points_on_unit_sphere.h"
        const auto& p = POINT_COORDINATES_ON_SPHERE;
        const size_t num_points = p.size();
        const real_number weight_of_each_point = real_number(4.0 * M_PI / num_points);
        real_number integral = 0.0;
        for(int i=0; i<num_points; i++){
            Vec3 dir = Vec3(p[i][0], p[i][1], p[i][2]);
            const auto sp = SphericalCoordinates(dir);
            if(sp.mPhi < GetBottomLimitPhi() ||  sp.mPhi > GetTopLimitPhi() || sp.mTheta < GetBottomLimitTheta() || sp.mTheta > GetTopLimitTheta()) {
                continue;
            }
            const real_number E_phi = GetDirectionalPhiPolarizationElectricFieldValue(sp);
            const real_number E_theta = GetDirectionalThetaPolarizationElectricFieldValue(sp);
            const real_number power_density = (E_phi*E_phi +  E_theta*E_theta) * INVERSE_OF_IMPEDANCE_OF_FREE_SPACE;
            integral += power_density;
        }
        integral *= weight_of_each_point;

        return integral;
    }

    virtual inline real_number GetBottomLimitPhi() const override {
        return -90.0;
    }

    virtual inline real_number GetTopLimitPhi() const override {
        return 90.0;
    }

};


#endif