#ifndef __Ray_ittest
#define __Ray_ittest

#include <limits>
#include <string>
#include <iostream>
#include "../src/constants.h"
#include "../src/vector.h"
#include "../src/jones.h"

class Test {

    public:

    Test(){};
    unsigned int mNumber = 0;
    virtual bool Run(){
        return 1;
    }
    static bool CheckIfValuesAreEqual(const real_number& a, const real_number& b) {
        if(std::abs(a - b) < TOLERANCE_FOR_TESTS){
            return true;
        }
        else {
            std::cout<<a<<" is not equal to "<<b<<std::endl;
            return false;
        }
    }

    static bool CheckIfValuesAreEqual(const Vec3& a, const Vec3& b) {
        for(int i=0; i<3; i++){
            if(std::abs(a[i] - b[i]) > TOLERANCE_FOR_TESTS){
                std::cout<<a[i]<<" is not equal to "<<b[i]<<" when comparing component "<<i<<" of Vec3"<<std::endl;
                return false;
            }
        }
        return true;
    }

    static bool CheckIfValuesAreEqual(const JonesVector& a, const JonesVector& b) {
        for(int i=0; i<3; i++){
            if(std::abs(a.mX[i] - b.mX[i]) > TOLERANCE_FOR_TESTS ||
            std::abs(a.mY[i] - b.mY[i]) > TOLERANCE_FOR_TESTS ){
                std::cout<<"Orientation of JonesVector's are not equal!"<<std::endl;
                return false;
            }
        }
        if(! CheckIfValuesAreEqual(a.mWaves[0], b.mWaves[0])) return false;
        if(! CheckIfValuesAreEqual(a.mWaves[1], b.mWaves[1])) return false;

        return true;
    }

    static bool CheckIfValuesAreEqual(const OrientedJonesVector& a, const OrientedJonesVector& b) {

        for(int i=0; i<3; i++){
            if(a.mOrientedVectorSum[i].real() - b.mOrientedVectorSum[i].real() > TOLERANCE_FOR_TESTS) {
                std::cout<<"OrientedJonesVector's are not equal!"<<std::endl;
                return false;
            }
            if(a.mOrientedVectorSum[i].imag() - b.mOrientedVectorSum[i].imag() > TOLERANCE_FOR_TESTS) {
                std::cout<<"OrientedJonesVector's are not equal!"<<std::endl;
                return false;
            }
        }

        if(! CheckIfValuesAreEqual(a.mFrequency, b.mFrequency)) {
            std::cout<<"OrientedJonesVector's are not equal!"<<std::endl;
            return false;
        }
        if(! CheckIfValuesAreEqual(a.mReferenceDistance, b.mReferenceDistance)) {
            std::cout<<"OrientedJonesVector's are not equal!"<<std::endl;
            return false;
        }

        return true;
    }

    static bool CheckIfValuesAreEqual(const Wave& a, const Wave& b) {

        if(! CheckIfValuesAreEqual(a.mFrequency, b.mFrequency)) return false;
        if(! CheckIfValuesAreEqual(a.mAmplitude, b.mAmplitude)) return false;
        if(! CheckIfValuesAreEqual(a.mPhase, b.mPhase)) return false;
        if(! CheckIfValuesAreEqual(a.mReferenceDistance, b.mReferenceDistance)) return false;

        return true;
    }

    static bool CheckMeshResultsAreEqualToReference(const std::string& results_mesh_file_name, const std::string& mesh_reference_file_name);
};

#endif