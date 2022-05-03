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
        mRadiationMap.resize(int(360.0 / mSeparationBetweenPhiValues) + 1);
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

};


#endif