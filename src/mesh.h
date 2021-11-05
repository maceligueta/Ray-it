#ifndef __Ray_it__Mesh__
#define __Ray_it__Mesh__

#include <stdio.h>
#include <vector>
#include "box.h"
#include "triangle.h"
#include "kdTree.h"
#include "node.h"

class Mesh;

class Mesh {
public:
    Vec3 mBoundingBox[2];
    KDTreeNode mTree;
    std::vector<Node> mNodes;
    std::vector<Triangle*> mTriangles;
    long mTriangleCount;

    Mesh(){};

    bool IntersectBoundingBox(Ray &ray) const;
    bool Intersect(Ray &ray) const;

};
#endif /* defined(__Ray_it__Mesh__) */
