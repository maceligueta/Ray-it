#ifndef __Ray_itnode
#define __Ray_itnode

class Node : public Vec3 {

    public:

    Node(){};
    Node(const Vec3& pos):Vec3(pos){}

    real_number mIntensity = 0.0;
};

#endif