#ifndef RayTracer_node
#define RayTracer_node

class Node : public Vec3f {

    public:

    Node(){};
    Node(const Vec3f& pos):Vec3f(pos){}

    float mIntensity = 0.0;
};

#endif