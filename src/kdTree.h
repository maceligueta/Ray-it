#ifndef __BasicRayTracer__kdTree__
#define __BasicRayTracer__kdTree__

#include <stdio.h>
#include "box.h"
#include "triangle.h"

struct SplitPlane {
    SplitPlane(const int axis, const real_number pos): mAxis(axis), mPos(pos){};
    SplitPlane(){};

    int mAxis; // 0=x, 1=y, 2=z;
    real_number mPos;

    bool operator==(const SplitPlane& sp) {
        return(mAxis == sp.mAxis && mPos == sp.mPos);
    }
};

class KDTreeNode{
public:
    KDTreeNode():mLeaf(false), mTriangles(std::vector<Triangle*>()){};

    KDTreeNode& operator =(const KDTreeNode & other){
        mLeft = other.mLeft;
        mRight = other.mRight;
        mLeaf = other.mLeaf;
        mSplitPlane = other.mSplitPlane;
        mBounds = other.mBounds;
        mTriangles = other.mTriangles;
        return *this;
    }

    KDTreeNode *mLeft;
    KDTreeNode *mRight;
    bool mLeaf;
    SplitPlane mSplitPlane;
    Box mBounds;
    std::vector<Triangle*> mTriangles;

    bool isDone(size_t N, real_number minCv) const;
    real_number getSplitPos();
    void build(KDTreeNode &node);
    bool intersects(Triangle *triangle);
    void intersectAllTriangles(Ray &r);
    real_number calculateCost(const real_number &position);
    void Traverse(Ray &r) const;
    void Traverse(Ray &r, real_number t_near, real_number t_far) const;
    void splitBox(const Box& V, const SplitPlane& p, Box& VL, Box& VR) const;
    typedef enum { LEFT=-1, RIGHT=1, UNKNOWN=0 } PlaneSide;
    void SAH(const SplitPlane& p, const Box& V, size_t NL, size_t NR, size_t NP, real_number& CP, PlaneSide& pside) const;
    void findPlane(const std::vector<Triangle *>& T, const Box& V, SplitPlane& p_est, real_number& C_est, PlaneSide& pside_est) const;
    void DistributeTriangles(const std::vector<Triangle*>& T, const SplitPlane& p, const PlaneSide& pside, std::vector<Triangle*>& TL, std::vector<Triangle*>& TR) const;
    KDTreeNode* RecursiveTreeNodeBuild(const std::vector<Triangle *>& T, const Box &V, int depth, const SplitPlane& prev_plane);
};




#endif /* defined(__BasicRayTracer__kdTree__) */
