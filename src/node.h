#ifndef RayTracer_node
#define RayTracer_node

class Node : public Vec3 {

    public:

    Node(){};
    Node(const Vec3& pos):Vec3(pos){}

    real_number mIntensity = 0.0;
};

#endif