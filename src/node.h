#ifndef __Ray_itnode
#define __Ray_itnode

class Node : public Vec3 {

    public:
    int mId;
    real_number mIntensity = 0.0;

    Node(const int id, const Vec3& pos):Vec3(pos){
        mId = id;
    }
    Node(const int id, const real_number c0, const real_number c1, const real_number c2):Node(id, Vec3(c0, c1, c2)) {}

};

#endif