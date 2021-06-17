#ifndef __BasicRayTracer__Mesh__
#define __BasicRayTracer__Mesh__

#include <stdio.h>
#include <vector>
#include "box.h"
#include "triangle.h"
#include "kdTree.h"
#include "node.h"

class Mesh;

class Mesh {
public:
    Vec3f mBoundingBox[2];
    KDTreeNode mTree;
    std::vector<Node> mNodes;
    std::vector<Triangle*> mTriangles;
    std::vector<Vec3f> mNormals;
    long mTriangleCount;

    Mesh(){};

    bool IntersectBoundingBox(Ray &ray) const;
    bool Intersect(Ray &ray) const;

};
#endif /* defined(__BasicRayTracer__Mesh__) */
